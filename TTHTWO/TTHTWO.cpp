// TTHTWO.cpp 

#include <iostream>
#include <string>
#include <TTH/tth.h>
#include <TTH/tiger.h>
#include <TTH/byte_order.h>
#include <windows.h>
#include <fstream>
#include <string>

void byte_to_base32(char* dest, const unsigned char* src, unsigned len, int upper_case)
{
    const char a = (upper_case ? 'A' : 'a');
    unsigned shift = 0;
    unsigned char word;
    const unsigned char* e = src + len;
    while (src < e) {
        if (shift > 3) {
            word = (*src & (0xFF >> shift));
            shift = (shift + 5) % 8;
            word <<= shift;
            if (src + 1 < e)
                word |= *(src + 1) >> (8 - shift);
            ++src;
        }
        else {
            shift = (shift + 5) % 8;
            word = (*src >> ((8 - shift) & 7)) & 0x1F;
            if (shift == 0) src++;
        }
        *dest++ = (word < 26 ? word + a : word + '2' - 26);
    }
    *dest = '\0';
}


int main(int argc, char* argv[])
{
    
        // init TTH
        tth_ctx ctx;
        unsigned char hash[tiger_hash_length];
        tth_init(&ctx);
    
        // buffer
        const int BUFFER_SIZE = 65536;
        unsigned char buf[BUFFER_SIZE];
        DWORD dwBytesRead;
        std::cout << "Load File..." << std::endl;
    
    if (argc > 0) {
        try {

            int size_needed = MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, NULL, 0);
            wchar_t* wide_filename = new wchar_t[size_needed];
            MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, wide_filename, size_needed);

            HANDLE hFile;
            
            std::wcout << L"Wide character file name (UTF-16): " << wide_filename << std::endl;

            hFile = CreateFileW(wide_filename,
                GENERIC_READ, // open for reading
                0, // do not share
                NULL, // default security
                OPEN_EXISTING, // existing file only
                FILE_ATTRIBUTE_NORMAL, // normal file
                NULL);

            do
            {
                if (!ReadFile(hFile, buf, BUFFER_SIZE, &dwBytesRead, NULL)) {
                    std::cerr << "Error reading file. Error code: "
                        << GetLastError() << std::endl;
                    CloseHandle(hFile);
                    return 1;
                }
                
                if (dwBytesRead < 1) 
                {
                    break;
                }

                // Processing of the read data (hash update)
                tth_update(&ctx, buf, dwBytesRead);


            } while (true);

            CloseHandle(hFile);
            std::cout << "File read successfully." << std::endl;
            tth_final(&ctx, hash);
            
            char TTH[40] = { 0 }; // 39 characters + zero termination

            byte_to_base32(TTH, hash, 24, 1);
            
            // Base32 output
            std::cout << "Base32 encoded string: " << TTH << std::endl;


            // Change the original file name to a new one with a ".TTH" extension
            std::wstring wide_filename_tth = wide_filename;
            size_t dot_pos = wide_filename_tth.find_last_of(L'.');
            if (dot_pos != std::wstring::npos) {
                wide_filename_tth = wide_filename_tth.substr(0, dot_pos); // Remove old ending
            }
            wide_filename_tth += L".TTH"; // Add new ending


            // Write the Base32 string to the file
            std::wofstream outfile(wide_filename_tth, std::ios::out | std::ios::trunc);
            if (!outfile) {
                std::cerr << "Error: file " << std::string(wide_filename_tth.begin(), wide_filename_tth.end())
                    << " could not be created." << std::endl;
                delete[] wide_filename;
                return 1;
            }

            outfile << TTH; // Write the Base32 string to the file
            outfile.close();

            std::wcout << L"Base32 hash was saved to file: " << wide_filename_tth << std::endl;

            delete[] wide_filename; // Free memory


            return 0;
                   
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Error: " << std::endl;
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Error: " << std::endl;
        }
    }
    else {
        std::cerr << "Error:" << std::endl;
    }

    return 0;
}


