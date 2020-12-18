/*
main.cpp
CSPB 1300 Image Processing Application

PLEASE FILL OUT THIS SECTION PRIOR TO SUBMISSION

- Your name:
    Jacob S. Unger

- All project requirements fully met? (YES or NO):
    Yes!

- If no, please explain what you could not get to work:
    Thanks for answering questions on your day off!

- Did you do any optional enhancements? If so, please explain:
    I included a negative filter. This was actually the first function I wrote because it seemed like a good template or kickoff point to begin thinking about the other required functions. I anjoyed this project so much that I included a star wars option on the menu! I certainly will continue to add features to this program since doing just the base requirments taught me so much and really solidified some of the concepts that we learned in class but only really tested in very specific cases! Happy holidays Mr. Kobayashi!
*/
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <initializer_list>
#include <cstring>
using namespace std;

//***************************************************************************************************//
//                                DO NOT MODIFY THE SECTION BELOW                                    //
//***************************************************************************************************//

// Pixel structure
struct Pixel
{
    // Red, green, blue color values
    int red;
    int green;
    int blue;
};

/**
 * Gets an integer from a binary stream.
 * Helper function for read_image()
 * @param stream the stream
 * @param offset the offset at which to read the integer
 * @return the integer starting at the given offset
 */ 
int get_int(fstream& stream, int offset)
{
    stream.seekg(offset);
    int result = 0;
    int base = 1;
    for (int i = 0; i < 4; i++)
    {   
        result = result + stream.get() * base;
        base = base * 256;
    }
    return result;
}
/**
 * Reads the BMP image specified and returns the resulting image as a vector
 * @param filename BMP image filename
 * @return the image as a vector of vector of Pixels
 */
vector<vector<Pixel>> read_image(string filename)
{
    // Open the binary file
    fstream stream;
    stream.open(filename, ios::in | ios::binary);

    // Get the image properties
    int file_size = get_int(stream, 2);
    int start = get_int(stream, 10);
    int width = get_int(stream, 18);
    int height = get_int(stream, 22);

    // Scan lines must occupy multiples of four bytes
    int scanline_size = width * 3;
    int padding = 0;
    if (scanline_size % 4 != 0)
    {
        padding = 4 - scanline_size % 4;
    }

    // Return empty vector if this is not a valid 24-bit true color image
    if (file_size != start + (scanline_size + padding) * height)
    {
        return {};
    }

    // Create a vector the size of the input image
    vector<vector<Pixel>> image(height, vector<Pixel> (width));

    int pos = start;
    // For each row, starting from the last row to the first
    // Note: BMP files store pixels from bottom to top
    for (int i = height - 1; i >= 0; i--)
    {
        // For each column
        for (int j = 0; j < width; j++)
        {
            // Go to the pixel position
            stream.seekg(pos);

            // Save the pixel values to the image vector
            // Note: BMP files store pixels in blue, green, red order
            image[i][j].blue = stream.get();
            image[i][j].green = stream.get();
            image[i][j].red = stream.get();

            // Advance the position to the next pixel
            pos = pos + 3;
        }

        // Skip the padding at the end of each row
        stream.seekg(padding, ios::cur);
        pos = pos + padding;
    }

    // Close the stream and return the image vector
    stream.close();
    return image;
}

/**
 * Sets a value to the char array starting at the offset using the size
 * specified by the bytes.
 * This is a helper function for write_image()
 * @param arr    Array to set values for
 * @param offset Starting index offset
 * @param bytes  Number of bytes to set
 * @param value  Value to set
 * @return nothing
 */
void set_bytes(unsigned char arr[], int offset, int bytes, int value)
{
    for (int i = 0; i < bytes; i++)
    {
        arr[offset+i] = (unsigned char)(value>>(i*8));
    }
}

/**
 * Write the input image to a BMP file name specified
 * @param filename The BMP file name to save the image to
 * @param image    The input image to save
 * @return True if successful and false otherwise
 */
bool write_image(string filename, const vector<vector<Pixel>>& image)
{
    // Get the image width and height in pixels
    int width_pixels = image[0].size();
    int height_pixels = image.size();

    // Calculate the width in bytes incorporating padding (4 byte alignment)
    int width_bytes = width_pixels * 3;
    int padding_bytes = 0;
    padding_bytes = (4 - width_bytes % 4) % 4;
    width_bytes = width_bytes + padding_bytes;

    // Pixel array size in bytes, including padding
    int array_bytes = width_bytes * height_pixels;

    // Open a file stream for writing to a binary file
    fstream stream;
    stream.open(filename, ios::out | ios::binary);

    // If there was a problem opening the file, return false
    if (!stream.is_open())
    {
        return false;
    }

    // Create the BMP and DIB Headers
    const int BMP_HEADER_SIZE = 14;
    const int DIB_HEADER_SIZE = 40;
    unsigned char bmp_header[BMP_HEADER_SIZE] = {0};
    unsigned char dib_header[DIB_HEADER_SIZE] = {0};

    // BMP Header
    set_bytes(bmp_header,  0, 1, 'B');              // ID field
    set_bytes(bmp_header,  1, 1, 'M');              // ID field
    set_bytes(bmp_header,  2, 4, BMP_HEADER_SIZE+DIB_HEADER_SIZE+array_bytes); // Size of BMP file
    set_bytes(bmp_header,  6, 2, 0);                // Reserved
    set_bytes(bmp_header,  8, 2, 0);                // Reserved
    set_bytes(bmp_header, 10, 4, BMP_HEADER_SIZE+DIB_HEADER_SIZE); // Pixel array offset

    // DIB Header
    set_bytes(dib_header,  0, 4, DIB_HEADER_SIZE);  // DIB header size
    set_bytes(dib_header,  4, 4, width_pixels);     // Width of bitmap in pixels
    set_bytes(dib_header,  8, 4, height_pixels);    // Height of bitmap in pixels
    set_bytes(dib_header, 12, 2, 1);                // Number of color planes
    set_bytes(dib_header, 14, 2, 24);               // Number of bits per pixel
    set_bytes(dib_header, 16, 4, 0);                // Compression method (0=BI_RGB)
    set_bytes(dib_header, 20, 4, array_bytes);      // Size of raw bitmap data (including padding)                     
    set_bytes(dib_header, 24, 4, 2835);             // Print resolution of image (2835 pixels/meter)
    set_bytes(dib_header, 28, 4, 2835);             // Print resolution of image (2835 pixels/meter)
    set_bytes(dib_header, 32, 4, 0);                // Number of colors in palette
    set_bytes(dib_header, 36, 4, 0);                // Number of important colors

    // Write the BMP and DIB Headers to the file
    stream.write((char*)bmp_header, sizeof(bmp_header));
    stream.write((char*)dib_header, sizeof(dib_header));

    // Initialize pixel and padding
    unsigned char pixel[3] = {0};
    unsigned char padding[3] = {0};

    // Pixel Array (Left to right, bottom to top, with padding)
    for (int h = height_pixels - 1; h >= 0; h--)
    {
        for (int w = 0; w < width_pixels; w++)
        {
            // Write the pixel (Blue, Green, Red)
            pixel[0] = image[h][w].blue;
            pixel[1] = image[h][w].green;
            pixel[2] = image[h][w].red;
            stream.write((char*)pixel, 3);
        }
        // Write the padding bytes
        stream.write((char *)padding, padding_bytes);
    }

    // Close the stream and return true
    stream.close();
    return true;
}

//***************************************************************************************************//
//                                DO NOT MODIFY THE SECTION ABOVE                                    //
//***************************************************************************************************//


//
// YOUR FUNCTION DEFINITIONS BELOW
//

//negative function 
vector<vector<Pixel>> process_11(const vector<vector<Pixel>>& image)
{
    int row_size = image.size(); 
    int column_size = image[0].size(); 
    vector<vector<Pixel>> result(row_size, vector<Pixel> (column_size)); 
    for(int i = 0; i < row_size; i++)
    {
        for(int j = 0; j < column_size; j++) 
        {
            int neg_red = 255 - image[i][j].red; 
            int neg_green = 255 - image[i][j].green; 
            int neg_blue = 255 - image[i][j].blue;
            result[i][j].red = neg_red; 
            result[i][j].green = neg_green;
            result[i][j].blue = neg_blue; 
        }
        
    }
    return result;
}


//process 1: vignette
vector<vector<Pixel>> process_1(const vector<vector<Pixel>>& image)
{
    int row_size = image.size(); 
    int column_size = image[0].size(); 
    vector<vector<Pixel>> result(row_size, vector<Pixel> (column_size)); 
    for(int i = 0; i < row_size; i++)
    {
        for(int j = 0; j < column_size; j++) 
        {
            double distance = sqrt(((pow(((j - row_size)/2), 2))) + ((pow(((i - column_size)/2), 2))));
            double scaling_factor = ((column_size - distance)/column_size);
            int red = image[i][j].red;
            int green = image[i][j].green;
            int blue = image[i][j].blue;
            result[i][j].red = red * scaling_factor;
            result[i][j].green = green * scaling_factor;
            result[i][j].blue = blue * scaling_factor;
        }
        
    }
    return result;
}    
    
    
  
//process 2: Claredon (darken the darks and lighten the lights) 
vector<vector<Pixel>> process_2(const vector<vector<Pixel>>& image, double scaling_factor)
{ 
    int row_size = image.size(); 
    int column_size = image[0].size(); 
    vector<vector<Pixel>> result(row_size, vector<Pixel> (column_size)); 
    for(int i = 0; i < row_size; i++) 
    {
        for(int j = 0; j < column_size; j++) 
        {
            int red = image[i][j].red;
            int green = image[i][j].green;
            int blue = image[i][j].blue;
            int avg_col = ((red + green + blue)/3);
            if (avg_col >= 170)
            {
                result[i][j].red = (255 - (255 - red) * scaling_factor);
                result[i][j].green = (255 - (255 - green) * scaling_factor);
                result[i][j].blue = (255 - (255 - blue) * scaling_factor);
            }
            else if (avg_col < 90)
            {
                result[i][j].red = red * scaling_factor;
                result[i][j].green = green * scaling_factor;
                result[i][j].blue = blue * scaling_factor;
            }
            else
            {
                result[i][j].red = red;
                result[i][j].green = green;
                result[i][j].blue = blue;
            }
        }     
    }
    return result;
}
    

     
//Process 3: greyscale 
vector<vector<Pixel>> process_3(const vector<vector<Pixel>>& image)
{ 
    int row_size = image.size(); 
    int column_size = image[0].size(); 
    vector<vector<Pixel>> result(row_size, vector<Pixel> (column_size)); 
    for(int i = 0; i < row_size; i++) 
    {
        for(int j = 0; j < column_size; j++) 
        {
           int red = image[i][j].red;
           int green = image[i][j].green;
           int blue = image[i][j].blue;
           int grey_value = ((red + green + blue)/3);
           result[i][j].red = grey_value; 
           result[i][j].green = grey_value;
           result[i][j].blue = grey_value;
        }
        
    }
    return result;
}
    
  

//process 4: 90 degree rotation
vector<vector<Pixel>> process_4(const vector<vector<Pixel>>& image)
{ 
    int row_size = image.size(); 
    int column_size = image[0].size(); 
    vector<vector<Pixel>> result(column_size, vector<Pixel> (row_size));
    for(int row = 0; row < row_size; row++)
    {
        for(int col = 0; col < column_size; col++)
        {
            int red = image[row][col].red;
            int green = image[row][col].green;
            int blue = image[row][col].blue;
            
            result[col][(row_size - 1) - row].red = red;
            result[col][(row_size - 1) - row].green = green;
            result[col][(row_size - 1) - row].blue = blue;
        }
    }
    return result;
}  
    
     
//process 5: rotation by a multiple of 90 degrees
vector<vector<Pixel>> process_5(const vector<vector<Pixel>>& image, int number)
{
    int angle = number * 90;
    if(angle%90 != 0)
    {
        cout << "Angle must be a multiple of 90 degrees" << endl;
    }
    else if (angle%360 == 0)
    {
        return image;
    }
    else if (angle%360 == 90)
    {
        return process_4(image);
    }
    else if (angle%360 == 180)
    {
        return process_4(process_4(image));
    }
    else
    {
        return process_4(process_4(process_4(image)));
    }
    
}
    
    
       
//process 6: enlarges in input x and y directions
vector<vector<Pixel>> process_6(const vector<vector<Pixel>>& image, int x_scale, int y_scale)    
{ 
    int row_size = image.size(); 
    int column_size = image[0].size(); 
    vector<vector<Pixel>> result(row_size * y_scale, vector<Pixel> (column_size * x_scale));  
    for(int row = 0; row < y_scale * row_size; row++) 
    {
        for(int col = 0; col < x_scale * column_size; col++) 
        {
            int red = image[row/y_scale][col/x_scale].red;
            int green = image[row/y_scale][col/x_scale].green;
            int blue = image[row/y_scale][col/x_scale].blue;
            
            result[row][col].red = red;
            result[row][col].green = green;
            result[row][col].blue = blue;
        }     
   }
    return result;
}    
       
    
       
//process 7: Convert image to high contrast (black and white only) 
vector<vector<Pixel>> process_7(const vector<vector<Pixel>>& image)
{ 
    int row_size = image.size(); 
    int column_size = image[0].size(); 
    double switch_val = 255/2;
    vector<vector<Pixel>> result(row_size, vector<Pixel> (column_size));  
    for(int i = 0; i < row_size; i++) 
    {
        for(int j = 0; j < column_size; j++) 
        {
            int red = image[i][j].red;
            int green = image[i][j].green;
            int blue = image[i][j].blue;
            int grey_val = ((red + green + blue)/3);
            if(grey_val >= switch_val)
            {
                result[i][j].red = 255;
                result[i][j].green = 255;
                result[i][j].blue = 255;
            }
            else
            {
                result[i][j].red = 0;
                result[i][j].green = 0;
                result[i][j].blue = 0;
            }
        }     
    }
    return result;
}    
    
    
    
//process 8: Lightens image by a scaling factor
vector<vector<Pixel>> process_8(const vector<vector<Pixel>>& image, double scaling_factor)
{ 
    int row_size = image.size(); 
    int column_size = image[0].size(); 
    vector<vector<Pixel>> result(row_size, vector<Pixel> (column_size)); 
    for(int i = 0; i < row_size; i++) 
    {
        for(int j = 0; j < column_size; j++) 
        {
            int red = image[i][j].red;
            int green = image[i][j].green;
            int blue = image[i][j].blue;
            result[i][j].red = (255 - (255 - red) * scaling_factor);
            result[i][j].green = (255 - (255 - green) * scaling_factor);
            result[i][j].blue = (255 - (255 - blue) * scaling_factor);
        }     
    }
    return result;
}    
    
    
   
//process 9: Darkens image by a scaling factor
vector<vector<Pixel>> process_9(const vector<vector<Pixel>>& image, double scaling_factor)
{ 
    int row_size = image.size(); 
    int column_size = image[0].size(); 
    vector<vector<Pixel>> result(row_size, vector<Pixel> (column_size)); 
    for(int i = 0; i < row_size; i++) 
    {
        for(int j = 0; j < column_size; j++) 
        {
            int red = image[i][j].red;
            int green = image[i][j].green;
            int blue = image[i][j].blue;
            result[i][j].red = (red * scaling_factor);
            result[i][j].green = (green * scaling_factor);
            result[i][j].blue = (blue * scaling_factor);
        }     
    }
    return result;
}     
    
      
//process 10: Converts image to only black, white, red, blue, and green 
vector<vector<Pixel>> process_10(const vector<vector<Pixel>>& image)
{ 
    int row_size = image.size(); 
    int column_size = image[0].size();
    vector<vector<Pixel>> result(row_size, vector<Pixel> (column_size));  
    for(int i = 0; i < row_size; i++)
    {
        for(int j = 0; j < column_size; j++) 
        {
            int red = image[i][j].red;
            int green = image[i][j].green;
            int blue = image[i][j].blue;
            int max_color = max({red, green, blue});
            if((red + green + blue) >= 550)
            {
                result[i][j].red = 255;
                result[i][j].green = 255;
                result[i][j].blue = 255;
            }
            else if((red + blue + green) <= 150)
            {
                result[i][j].red = 0;
                result[i][j].green = 0;
                result[i][j].blue = 0;
            }
            else if(max_color == red)
            {
                result[i][j].red = 255;
                result[i][j].green = 0;
                result[i][j].blue = 0;
            }
            else if(max_color == green)
            {
                result[i][j].red = 0;
                result[i][j].green = 255;
                result[i][j].blue = 0;
            }
            else
            {
                result[i][j].red = 0;
                result[i][j].green = 0;
                result[i][j].blue = 255;
            }
        }     
    }
    return result;
}    
    
//DRIVER 
int main()
{
    cout << endl;
    cout << endl;
    string in_fl_nm;
    string out_fl_nm;
    char ysno;
    char selection[5];
    double scaling_factor;
    double num_rotations;
    double x_scale;
    double y_scale;
    int user_continue = 1;
    cout << "CSPB 1300 Basic Image Processing Application" << endl;
    cout << "Please enter input BMP file to process: " << endl;
    cin >> in_fl_nm;
    while(user_continue == 1)
    {
        cout << endl;
        cout << "The image you are working with is " + in_fl_nm<< endl;
        cout << "IMAGE PROCESSING MENU" << endl;
        cout << "0) Change Current Image (Current image: " + in_fl_nm + ")" << endl;
        cout << "1) Vignette effect" << endl;
        cout << "2) Clarendon effect" << endl;
        cout << "3) Grayscale effect" << endl;
        cout << "4) Rotate image 90 degree's" << endl;
        cout << "5) Rotate Image a multiple of 90 degree's" << endl;
        cout << "6) Enlarge Image" << endl;
        cout << "7) High Contrast Effect" << endl;
        cout << "8) Lighten Image" << endl;
        cout << "9) Darken Image" << endl;
        cout << "10) Crazy Color Filter" << endl;
        cout << "11) Negative Filter" << endl;
        cout << endl;
        cout << "Enter Menu Selection, or 'Q' to quit: " << endl;
        cin >> selection;
        if (strcmp(selection,"0") == 0)
        {
            cout << "Enter new file here: " << endl;
            cin >> in_fl_nm;
            cout << endl;
            cout << "File change successful!" << endl;
        }
        else if(strcmp(selection,"1") == 0)
        {
            cout << "What would you like your output BMP file saved as? Input here: " << endl;
            cin >> out_fl_nm;
            cout << "Execute Process 1" << endl;
            vector<vector<Pixel>> input_vec = read_image(in_fl_nm);
            vector<vector<Pixel>> output_vec = process_1(input_vec);
            bool success = write_image(out_fl_nm, output_vec);
            cout << "Process Successful" << endl;
        }
        else if(strcmp(selection,"2") == 0)
        {
            cout << "What would you like your output BMP file saved as? Input here: " << endl;
            cin >> out_fl_nm;
            cout << "Please enter a scaling factor: " << endl;
            cin >> scaling_factor;
            cout << "Execute Process 2 with scaling factor of: "<< endl;
            cout << scaling_factor << endl;
            vector<vector<Pixel>> input_vec = read_image(in_fl_nm);
            vector<vector<Pixel>> output_vec = process_2(input_vec, scaling_factor);
            bool success = write_image(out_fl_nm, output_vec);
            cout << "File change successful!" << endl;
        }
        else if(strcmp(selection,"3") == 0)
        {
            cout << "What would you like your output BMP file saved as? Input here: " << endl;
            cin >> out_fl_nm;
            cout << "Execute Process 3" << endl;
            vector<vector<Pixel>> input_vec = read_image(in_fl_nm);
            vector<vector<Pixel>> output_vec = process_3(input_vec);
            bool success = write_image(out_fl_nm, output_vec);
            cout << "File change successful!" << endl;
        }    
        else if(strcmp(selection,"4") == 0)
        {
            cout << "What would you like your output BMP file saved as? Input here: " << endl;
            cin >> out_fl_nm;
            cout << "Execute Process 4" << endl;
            vector<vector<Pixel>> input_vec = read_image(in_fl_nm);
            vector<vector<Pixel>> output_vec = process_4(input_vec);
            bool success = write_image(out_fl_nm, output_vec);
            cout << "File change successful!" << endl;
        }
        else if(strcmp(selection,"5") == 0)
        {
            cout << "What would you like your output BMP file saved as? Input here: " << endl;
            cin >> out_fl_nm;
            cout << "Enter number of desired 90 degree rotations: " << endl;
            cin >> num_rotations;
            cout << "Execute Process 5 with ";
            cout << num_rotations;
            cout << " number or rotations" << endl;
            vector<vector<Pixel>> input_vec = read_image(in_fl_nm);
            vector<vector<Pixel>> output_vec = process_5(input_vec, num_rotations);
            bool success = write_image(out_fl_nm, output_vec);
            cout << "File change successful!" << endl;
        }
        else if(strcmp(selection,"6") == 0)
        {
            cout << "What would you like your output BMP file saved as? Input here: " << endl;
            cin >> out_fl_nm;
            cout << "Please enter a scaling value for x: ";
            cin >> x_scale;
            cout << "Please enter a scaling value for y: ";
            cin >> y_scale;
            cout << "Execute Process 6" << endl;
            vector<vector<Pixel>> input_vec = read_image(in_fl_nm);
            vector<vector<Pixel>> output_vec = process_6(input_vec, x_scale, y_scale);
            bool success = write_image(out_fl_nm, output_vec);
            cout << "File change successful!" << endl;
        }
        else if(strcmp(selection,"7") == 0)
        {
            cout << "What would you like your output BMP file saved as? Input here: " << endl;
            cin >> out_fl_nm;
            cout << "Execute Process 7" << endl;
            vector<vector<Pixel>> input_vec = read_image(in_fl_nm);
            vector<vector<Pixel>> output_vec = process_7(input_vec);
            bool success = write_image(out_fl_nm, output_vec);
            cout << "File change successful!" << endl;
        }
        else if(strcmp(selection,"8") == 0)
        {
            cout << "What would you like your output BMP file saved as? Input here: " << endl;
            cin >> out_fl_nm;
            cout << "Enter desireed scaling factor: " << endl;
            cin >> scaling_factor;
            cout << "Execute Process 8 with scaling factor: "<< endl;
            cout << scaling_factor << endl;
            vector<vector<Pixel>> input_vec = read_image(in_fl_nm);
            vector<vector<Pixel>> output_vec = process_8(input_vec, scaling_factor);
            bool success = write_image(out_fl_nm, output_vec);
            cout << "File change successful!" << endl;
        }
        else if(strcmp(selection,"9") == 0)
        {
            cout << "What would you like your output BMP file saved as? Input here: " << endl;
            cin >> out_fl_nm;
            cout << "Enter desireed scaling factor: " << endl;
            cin >> scaling_factor;
            cout << "Execute Process 8 with scaling factor: " << endl;
            cout << scaling_factor << endl;
            vector<vector<Pixel>> input_vec = read_image(in_fl_nm);
            vector<vector<Pixel>> output_vec = process_9(input_vec, scaling_factor);
            bool success = write_image(out_fl_nm, output_vec);
            cout << "File change successful!" << endl;
        }
        else if(strcmp(selection,"10") == 0)
        {
            cout << "What would you like your output BMP file saved as? Input here: " << endl;
            cin >> out_fl_nm;
            cout << "Execute process 10" << endl;
            vector<vector<Pixel>> input_vec = read_image(in_fl_nm);
            vector<vector<Pixel>> output_vec = process_10(input_vec);
            bool success = write_image(out_fl_nm, output_vec);
            cout << "File change successful!" << endl;
        }
        else if(strcmp(selection,"11") == 0)
        {
            cout << "What would you like your output BMP file saved as? Input here: " << endl;
            cin >> out_fl_nm;
            cout << "Execute process 11" << endl;
            vector<vector<Pixel>> input_vec = read_image(in_fl_nm);
            vector<vector<Pixel>> output_vec = process_11(input_vec);
            bool success = write_image(out_fl_nm, output_vec);
            cout << "File change successful!" << endl;
        }
        else if((strcmp(selection,"Q") == 0) || (strcmp(selection,"q") == 0))
        {
            cout << "Are you sure you would like to Exit? (Y/N)" << endl;
            cin >> ysno;
            if(ysno == 'Y' || ysno == 'y')
            {
                cout << "Goodbye!" << endl;
                user_continue = 0;
            }
            else
            {
                user_continue = 1;
            }
        }
        else if(strcmp(selection,"66") == 0)
        {
            cout << endl;
            cout << endl;
            cout << endl;
            cout << endl;
            cout << "                              Execute Order 66                               " << endl;
            cout << endl;
            cout << endl;
            cout << ".    .        .      .             . .     .        .          .          .  " << endl;
            cout << "         .                 .                    .                .           " << endl;
            cout << "  .        .             The Empire did nothing wrong...              .      " << endl;
            cout << "     .               .           .               .        .             .    " << endl;
            cout << "     .      .            .                 .                                ." << endl;
            cout << " .      .         .         .   . :::::+::::...      .          .         .  " << endl;
            cout << "     .         .      .    ..::.:::+++++:::+++++:+::.    .     .       .     " << endl;
            cout << "                        .:.  ..:+:..+|||+..::|+|+||++|:.             .     . " << endl;
            cout << "            .   .    :::....:::::::::++||||O||O#OO|OOO|+|:.       .          " << endl;
            cout << ".      .      .    .:..:..::+||OO#|#|OOO+|O||####OO###O+:+|+               . " << endl;
            cout << "                 .:...:+||O####O##||+|OO|||O#####O#O||OO|++||:     .    .    " << endl;
            cout << "  .             ..::||+++|+++++|+::|+++++O#O|OO|||+++..:OOOOO|+  .         . " << endl;
            cout << "     .   .     +++||++:.:++:..+#|. ::::++|+++||++O##O+:.++|||#O+    .        " << endl;
            cout << ".           . ++++++++...:+:+:.:+: ::..+|OO++O|########|++++||##+           ." << endl;
            cout << "  .       .  :::+++|O+||+::++++:::+:::+++::+|+O###########OO|:+OO       .  . " << endl;
            cout << "     .       +:+++|OO+|||O:+:::::.. .||O#OOO||O||#@###@######:+|O|  .        " << endl;
            cout << " .          ::+:++|+|O+|||++|++|:::+O#######O######O@############O           " << endl;
            cout << "          . ++++: .+OO###O++++++|OO++|O#@@@####@##################+         ." << endl;
            cout << "      .     ::::::::::::::::::::++|O+..+#|O@@@@#@###O|O#O##@#OO####     .    " << endl;
            cout << " .        . :. .:.:. .:.:.: +.::::::::  . +#:#@:#@@@#O||O#O@:###:#| .      . " << endl;
            cout << "                           `. .:.:.:.:. . :.:.:%::%%%:::::%::::%:::          " << endl;
            cout << ".      .                                      `.:.:.:.:   :.:.:.:.  .   .    " << endl;
            cout << "           .                                                                ." << endl;
            cout << "    .                                                                        " << endl;
            cout << ".          .                                                       .   .     " << endl;
            cout << "    .        .                                                           .   " << endl;
            cout << "    .     .                                                          .      ." << endl;
            cout << "  .     .                                                        .           " << endl;
            cout << "           .                                                                ." << endl;
            cout << "     .               .           .               .        .             .    " << endl;
            cout << ".    .        .      .             . .     .        .          .          .  " << endl;
            cout << "  .        .  And Darth Vader will live to Rule the Galaxy...     .          " << endl;
            cout << "                ._,.                                                         " << endl;
            cout << "            '..-..pf.                                                        " << endl;
            cout << "          -L   ..#''                                                         " << endl;
            cout << "        .+_L  .'']#                                                          " << endl;
            cout << "        ,'j' .+.j`                 -'.__..,.,p.                              " << endl;
            cout << "       _~ #..<..0.                 .J-.``..._f.                              " << endl;
            cout << "      .7..#_.. _f.                .....-..,`4'                               " << endl;
            cout << "      ;` ,#j.  T'      ..         ..J....,'.j`           LONG LIVE           " << endl;
            cout << "     .` ..'^.,-0.,,,,yMMMMM,.    ,-.J...+`.j@            THE EMPIRE!         " << endl;
            cout << "    .'.`...' .yMMMMM0M@^=`""g.. .'..J..'.'.jH                                " << endl;
            cout << "    j' .'1`  q'^)@@#'^'.`'='BNg_...,]_)'...0-                                " << endl;
            cout << "   .T ...I. j'    .'..+,_.'3#MMM0MggCBf....F.'                               " << endl;
            cout << "   j/.+'.{..+       `^~'-^~~'''''''''?''`''1`                                " << endl;
            cout << "   .... .y.}                  `.._-:`_...jf                                  " << endl;
            cout << "   g-.  .Lg'                 ..,..'-....,'.                                  " << endl;
            cout << "  .'.   .Y^                  .....',].._f                                    " << endl;
            cout << " ......-f.                 .-,,.,.-:--&`                                     " << endl;
            cout << "                           .`...'..`_J`                                      " << endl;
            cout << "                           .~......'#'                                       " << endl;
            cout << "                           '..,,.,_]`                                        " << endl;
            cout << "                            .L..`..``.                                       " << endl;
            cout << endl;
            cout << endl;
            cout << " ASCII Art credit to Ray Brunner" << endl;                                 
        }
        else
        {
            cout << "Please enter an input from the menu" << endl;
        }
    }
    return 0;
}

    
