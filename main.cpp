//#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <filesystem>
#include <array>
#include <string>

constexpr char VERSION[] = "V. 1.0";

int main(int argc, char* argv[])
{
    if (argc != 7) {
        std::cout << "Invalid Usage!\n";
        std::cout << "FontToGlyphs " << VERSION << '\n';
        std::cout << "Usage: FontToGlyphs <Font File> <Font Size> <Output Directory>\n";
        std::cout << "\t-f, --file\tTTF Font File\n";
        std::cout << "\t-s, --size\tTTF Font Size\n";
        std::cout << "\t-d, --dir\tOutput Directory for font glyphs\n";
        return -1;
    }
    std::filesystem::path glyphDirPath;
    std::string fontPath;
    int fontSize = -1;

    for (int i = 0; i < argc; i++) {
        if ((std::string(argv[i]) == "-f") || (std::string(argv[i]) == "--file")) {
            if (i+1 < argc)
                fontPath = argv[i+1];
            else {
                std::cerr << "Missing font file argument!\n";
                return -1;
            }
        }
        else if ((std::string(argv[i]) == "-s") ||(std::string(argv[i]) == "--size")) {
            if (i+1 < argc)
                fontSize = std::atoi(argv[i+1]);
            else {
                std::cerr << "Missing font size argument!\n";
                return -1;
            }
        }
        else if ((std::string(argv[i]) == "-d") || (std::string(argv[i]) == "--dir")) {
            if (i+1 < argc)
                glyphDirPath = std::filesystem::path(argv[i+1]);
            else {
                std::cerr << "Missing output directory argument!\n";
                return -1;
            }
        }
    }

    if (std::filesystem::exists(glyphDirPath) && !std::filesystem::is_directory(glyphDirPath)) {
        std::cerr << "File: " << glyphDirPath << " already exists and is not a directory.\n";
        return -1;
    }
    if (!std::filesystem::exists(glyphDirPath) && !std::filesystem::create_directory(glyphDirPath, std::filesystem::current_path())) {
        std::cerr << "Failed to create directory: " << glyphDirPath << "\n";
        return -1;
    }

    //if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    //    std::cerr << "SDL Init Failed: " << SDL_GetError() << "\n";
    //    return -1;
    //}
    if (TTF_Init() < 0) {
        std::cerr << "TTF Init Failed: " << TTF_GetError() << "\n";
        return -1;
    }
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))) {
        std::cerr << "SDL_Image Init Failed: " << IMG_GetError() << "\n";
        return -1;
    }

    constexpr int gridSize = 24;
    SDL_Window* win = SDL_CreateWindow(fontPath.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       gridSize*fontSize, gridSize*fontSize, SDL_WINDOW_HIDDEN);
    if (!win) {
        std::cerr << "Failed to open SDL Window!\n";
        return -1;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    if (!ren) {
        std::cerr << "Failed to open SDL Renderer!\n";
        return -1;
    }


    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font) {
        std::cerr << "Failed to open TTF Font: " << TTF_GetError() << "\n";
        return -1;
    }

    SDL_RenderClear(ren);

    SDL_Rect destRect;
    //SDL_Texture* tex = nullptr;
    destRect.x = destRect.y = 0;
    destRect.w = destRect.h = fontSize;
    const SDL_Color white = { 255, 255, 255, 255 };
    const SDL_Color red = { 255, 0, 0, 0 };
    const SDL_Color green = { 0, 255, 0, 255 };
    const SDL_Color blue = { 0, 0, 255, 255 };

    std::array<SDL_Surface*, gridSize * gridSize> glyphSurfaces = { nullptr };
    std::array<SDL_Rect, gridSize * gridSize> glyphSurfaceRects = { { {0, 0, 0, 0}} };

    std::cout << "[Writing Glyphs to " << glyphDirPath << "]\n";

    for (int font_indx = 0; font_indx < gridSize * gridSize; font_indx++) {
        //SDL_DestroyTexture(tex);
        glyphSurfaces[font_indx] = TTF_RenderGlyph32_Blended(font, font_indx, white);
        glyphSurfaceRects[font_indx] ={ (font_indx % gridSize) * fontSize, (font_indx / gridSize) * fontSize, fontSize, fontSize };
        //tex = SDL_CreateTextureFromSurface(ren, glyphSurfaces[font_indx]);
        //SDL_RenderCopy(ren, tex, nullptr, &glyphSurfaceRects[font_indx]);
        std::string pngPath = std::string(glyphDirPath.c_str()) + "glyph" + std::to_string(font_indx) + ".png";

        if (glyphSurfaces[font_indx])
            IMG_SavePNG(glyphSurfaces[font_indx], pngPath.c_str());
    }

    std::cout << "[Finished Writing Glyphs!]\n";

    //SDL_RenderPresent(ren);    //present renderer
    //SDL_Delay(8000);

    for (uint8_t i = 0; i < 255; i++)
        SDL_FreeSurface(glyphSurfaces[i]);

    //SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
