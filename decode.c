#include <stdio.h>
#include <stdint.h>

// Function to decode JPEG file
void JFIF_DECODE(uint8_t *data, size_t size) {
    size_t pos = 0; // Position in the data array
    uint16_t height = 0;
    uint16_t width = 0;

    // Check for Start of Image (SOI) marker
    if (data[pos] != 0xFF || data[pos + 1] != 0xD8) {
        printf("Invalid JPEG file: Missing Start of Image (SOI) marker\n");
        return;
    }

    // Move to the next marker
    pos += 2;

    // Loop through the data array
    while (pos < size) {
        // Check for valid marker
        if (data[pos] != 0xFF) {
            printf("Invalid marker at position %zu\n", pos);
            return;
        }

        uint8_t marker = data[pos + 1]; // Marker type
        uint16_t length = (data[pos + 2] << 8) | data[pos + 3]; // Length of the marker segment

        // Print marker type and length
        printf("Marker: 0x%X, Length: %u\n", marker, length);

        // Handle marker types
        switch (marker) {
            case 0xC0: // SOF0 - Start of Frame (Baseline DCT)
                // Print SOF0 marker
                printf("Start of Frame (Baseline DCT)\n");
                // Extract and print image dimensions
                height = (data[pos + 5] << 8) | data[pos + 6];
                width = (data[pos + 7] << 8) | data[pos + 8];
                printf("Frame Width: %u, Height: %u\n", width, height);
                // Return after printing frame width and height
                return;

            case 0xDA: // SOS - Start of Scan
                // Print SOS marker
                printf("Start of Scan\n");
                // Move to the next marker (skip scan data)
                pos += length + 2;
                break;

            case 0xD9: // EOI - End of Image
                // Print EOI marker
                printf("End of Image\n");
                // End decoding process
                return;

            case 0xC4: // DHT - Define Huffman Table
                {
                    // Print DHT marker
                    printf("Define Huffman Table\n");
                    
                    // Process DHT marker data
                    uint8_t* dht_data = &data[pos + 2]; // Start of DHT data
                    
                    // Loop through DHT segments
                    size_t segment_pos = 0;
                    while (segment_pos < length) {
                        uint8_t table_info = dht_data[segment_pos];
                        uint8_t ac_dc = (table_info >> 4) & 0x0F; // AC or DC table
                        uint8_t table_id = table_info & 0x0F; // Table ID
                        printf("Huffman Table: %s, ID: %u\n", (ac_dc == 0) ? "DC" : "AC", table_id);
                        
                        // Parse the Huffman codes and store them for later decoding
                        // This step involves reading the lengths and values of the Huffman codes
                        // and constructing the Huffman trees for decoding.
                        
                        // Move to the next segment
                        segment_pos += 17; // Each DHT segment is 17 bytes long
                    }
                    
                    // Move to the next marker
                    pos += length + 2;
                    break;
                }

            case 0xDB: // DQT - Define Quantization Table
                // Print DQT marker
                printf("Define Quantization Table\n");
                // Handle DQT marker (you might need to parse quantization tables here)
                // Move to the next marker
                pos += length + 2;
                break;
            case 0xDD: // DRI - Define Restart Interval
                // Print DRI marker
                printf("Define Restart Interval\n");
                // Extract restart interval (two-byte parameter after the marker)
                uint16_t restart_interval = (data[pos + 2] << 8) | data[pos + 3];
                printf("Restart Interval: %u\n", restart_interval);
                // Move to the next marker
                pos += length + 2;
                break;

            case 0xE0: // APP0 - Application Segment 0
                // Print APP0 marker
                printf("Application Segment 0\n");
                // Handle APP0 marker (you might need to parse application-specific data here)
                // Move to the next marker
                pos += length + 2;
                break;
            case 0xE1: // APP1 - Application Segment 1 (usually contains EXIF data)
                // Print APP1 marker
                printf("Application Segment 1 (EXIF)\n");
                // Extract and process the contents of the APP1 segment
                // The segment data starts from data[pos + 4] and has a length of (length - 2) bytes
                // You can parse and handle the EXIF data here
                // Move to the next marker
                pos += length + 2;
                break;

            default:
                // Handle other markers
                printf("Unhandled marker: 0x%X\n", marker);
                // Move to the next marker
                pos += length + 2;
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    // Check if the file path is provided as command line argument
    if (argc != 2) {
        printf("Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    // Open the file
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory to store file data
    uint8_t *jpeg_data = (uint8_t *)malloc(file_size);
    if (jpeg_data == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return 1;
    }

    // Read file contents into the array
    size_t bytes_read = fread(jpeg_data, 1, file_size, file);
    if (bytes_read != file_size) {
        perror("Error reading file");
        free(jpeg_data);
        fclose(file);
        return 1;
    }

    // Close the file
    fclose(file);
    printf("file_size=%d\n",file_size);
    for(int j=0;j<64;j++)
    {
        printf("%x ",jpeg_data[j]);
    }
    printf("\n\n");
    for(int j=64;j<128;j++)
    {
        printf("%x ",jpeg_data[j]);
    }
    printf("\n\n");
    for(int j=128;j<196;j++)
    {
        printf("%x ",jpeg_data[j]);
    }
    printf("\n\n");
    // Call JFIF_DECODE function if it's a JFIF version
    if (jpeg_data[6] == 'J' && jpeg_data[7] == 'F' && jpeg_data[8] == 'I' && jpeg_data[9] == 'F') {
        printf("This is a JFIF version\n");
        JFIF_DECODE(jpeg_data, file_size);
    } else if (jpeg_data[6] == 'E' && jpeg_data[7] == 'X' && jpeg_data[8] == 'I' && jpeg_data[9] == 'F') 
    {
        printf("This is a EXIF version\n");
        // JFIF_DECODE(jpeg_data, file_size);
    }
    else{
        printf("This is not a JFIF version\n");
        printf("%c%c%c%c\n",jpeg_data[6] ,jpeg_data[7] ,jpeg_data[8] ,jpeg_data[9] );
        JFIF_DECODE(jpeg_data, file_size);
    }

    // Free allocated memory
    free(jpeg_data);

    return 0;
}
