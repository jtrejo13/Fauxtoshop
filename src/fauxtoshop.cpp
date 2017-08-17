// This is the CPP file you will edit and turn in.
// Also remove these comments here and add your own.
// TODO: rewrite this comment

#include <iostream>
#include "console.h"
#include "gwindow.h"
#include "grid.h"
#include "simpio.h"
#include "strlib.h"
#include "gbufferedimage.h"
#include "gevents.h"
#include "math.h" //for sqrt and exp in the optional Gaussian kernel
#include "gmath.h" // for sinDegrees(), cosDegrees(), PI
#include "random.h"
#include "tokenscanner.h"
using namespace std;

static const int    WHITE = 0xFFFFFF;
static const int    BLACK = 0x000000;
static const int    GREEN = 0x00FF00;

struct pixel{
    int row;
    int col;
};

struct coordinate{
    int x;
    int y;
};

struct RGBColor{
    int red;
    int green;
    int blue;
};

void     doFauxtoshop(GWindow &gw, GBufferedImage &img);

bool     openImageFromFilename(GBufferedImage& img, string filename);
bool 	 saveImageToFilename(const GBufferedImage &img, string filename);
void     getMouseClickLocation(int &row, int &col);
Vector<double> gaussKernelForRadius(int radius);

int getImageName(GBufferedImage &img);
int filterPrompt();
int jump(int choice, GBufferedImage &img);
void saveImage(GBufferedImage &img);

void scatterImage(GBufferedImage &img);
int valuePrompt(string & prompt, int lowerBound, int upperBound);
pixel getRandomPixel(int row, int col, int radius, GBufferedImage &img);

void edgeDetection(GBufferedImage &img);
RGBColor convertPixelColorToRGB(int pix);
int getMaxColorDifference(const RGBColor & cOne, const RGBColor & cTwo);
int getMaxNeighborDifference(int row, int col, Grid<int> & imageGrid);

void greenScreen(GBufferedImage &img);
void getStickerImage(GBufferedImage &img);
pixel pixelPrompt(string & prompt);
pixel stringToPoint(string s);

void rotation(GBufferedImage &img);
coordinate pixelToCoordinate(pixel p, GBufferedImage &img);
pixel coordinateToPoint(coordinate c, GBufferedImage &img);
coordinate rotateCoordinate(coordinate c, int angle);

void gaussianBlur(GBufferedImage &img);
int getWeightedColor(int pixel, double weight);


/* STARTER CODE FUNCTION - DO NOT EDIT
 *
 * This main simply declares a GWindow and a GBufferedImage for use
 * throughout the program. By asking you not to edit this function,
 * we are enforcing that the GWindow have a lifespan that spans the
 * entire duration of execution (trying to have more than one GWindow,
 * and/or GWindow(s) that go in and out of scope, can cause program
 * crashes).
 */
int main()
{
    GWindow gw;
    gw.setTitle("Fauxtoshop");
    gw.setVisible(true);
    GBufferedImage img;
    doFauxtoshop(gw, img);
    return 0;
}

/* This is yours to edit. Depending on how you approach your problem
 * decomposition, you will want to rewrite some of these lines, move
 * them inside loops, or move them inside helper functions, etc.
 *
 * TODO: rewrite this comment.
 */
void doFauxtoshop(GWindow &gw, GBufferedImage &img) {
    while(getImageName(img)){

        gw.setCanvasSize(img.getWidth(), img.getHeight());
        gw.add(&img,0,0);

        while(jump(filterPrompt(), img));
        gw.clear();
        cout << endl;
    }
    cout << "\nExiting Fauxtoshop. Goodbye... " << endl;
}

/*Prompts the user to enter an image name or "" to quit
Returns 1 if image was successfully opened
Returns 0 if user entered " " and would like to quit program*/
int getImageName(GBufferedImage &img)
{
    cout << "Welcome to Fauxtoshop!" << endl;
    while(true){
        string imageName = getLine("Enter name of image file to open (or blank to quit):");
        if(imageName == "") {return 0;}
        if(openImageFromFilename(img, imageName)){
            cout << "Opening image file. It may take a minute...\n" << endl;
            return 1;
        }else{
            cout << "Wrong file name... Please try again or enter blank to quit" << endl;
        }
    }
}

/*Prompts the user to select a filter
Returns user input if input is integer
Reprompts user for input if a non-integer is provided*/
int filterPrompt()
{
    cout << "Which image filter would you like to apply:\n";
    cout << "\t1 - Scatter\n";
    cout << "\t2 - Edge detection\n";
    cout << "\t3 - Green screen with another image\n";
    cout << "\t4 - Compare image with another image\n";
    cout << "\t5 - Rotation\n";
    cout << "\t6 - Gaussian Blur\n";
    cout << "\t0 - Quit";

    return  getInteger("\nYour choice", "\nINPUT ERROR: Enter a number between 0 and 6.");
}

/*Jump table to execute filter function based on user selection
Returns 1 and prints out error if input is outside range
Returns 0 if user's choice is to quit*/
int jump(int choice, GBufferedImage &img)
{
    if(choice < 0 || choice > 6){ //will change to functions.length
        cout << "\nINPUT ERROR: Enter a number between 0 and 6." <<endl;
        return 1;
    }else{
        switch (choice) {
        case 0:
            break;
        case 1:
            scatterImage(img);
            saveImage(img);
            break;
        case 2:
            edgeDetection(img);
            saveImage(img);
            break;
        case 3:
            greenScreen(img);
            saveImage(img);
            break;
        case 4:
            cout << "NOT IMPLEMENTED YET." << endl;
            break;
        case 5:
            rotation(img);
            saveImage(img);
            break;
        case 6:
            gaussianBlur(img);
            saveImage(img);
            break;
        default:
            cout << "Not a valid option." << endl;
            break;
        }
    }
    return 0;
}

/*Prompts user to enter a filename to save editted image
 Exits if user enters "" and saves image to file if a correct image name is entered*/
void saveImage(GBufferedImage &img)
{
    string fileName;
    do{
        fileName = getLine("Enter a filename to save image (or blank to skip saving): ");
        if(fileName == "") return;
    }while(!saveImageToFilename(img, fileName));
}

/************** SCATTER IMAGE *****************/

/*This functions takes a GBufferedImage and GWindow and “scatters” its pixels,
 * making something that looks like a sand drawing that was shaken.
The function prompts the user to provide a “degree of scatter”
for how far we should scatter pixels.
The value should be an integer between 1 and 100, inclusive (otherwise it reprompts).*/
void scatterImage(GBufferedImage &img)
{
    string prompt = "Enter degree of scatter [1-100]: ";
    int scatterDegree = valuePrompt(prompt, 1 , 100);

    Grid<int> imageGrid = img.toGrid();

    for(int r = 0; r < img.getHeight(); r++){
        for(int c = 0; c < img.getWidth(); c++){
            pixel rp = getRandomPixel(r, c, scatterDegree, img);
            imageGrid[r][c] = imageGrid[rp.row][rp.col];
        }
    }
    img.fromGrid(imageGrid);
}

/*Returns a randomly selected pixelexisting within the image bounds.
 The pixelis returned by randomly selecting a row within radius
 of the center pixel's row, and randomly selecting a column within radius of
 the center pixel's column.*/
pixel getRandomPixel(int row, int col, int radius, GBufferedImage &img)
{
   int rRow, rCol; //random row and column
   do{
       rRow = randomInteger(row - radius, row + radius);
       rCol = randomInteger(col - radius, col + radius);
   }while(rRow < 0 || rRow > img.getHeight() - 1 || rCol < 0 || rCol > img.getWidth() - 1);
   pixel randomP = {rRow, rCol};
   return randomP;
}

/************* EDGE DETECTION ****************/

/* TODO: PROVIDE DESCRIPTION */
void edgeDetection(GBufferedImage &img)
{
    string prompt = "Enter threshold for edge detection: ";
    int threshold = valuePrompt(prompt, 1, 100000);

    Grid<int> imageGrid = img.toGrid();
    Grid<int> bwGrid(img.getHeight(), img.getWidth());

    for(int r = 0; r < img.getHeight(); r++){
        for(int c = 0; c < img.getWidth(); c++){
            if(getMaxNeighborDifference(r, c, imageGrid) > threshold){ //if pixel is an edge
                bwGrid[r][c] = BLACK;
            }else{
                bwGrid[r][c] = WHITE;
            }
        }
    }

    img.fromGrid(bwGrid);
}

/* TODO: Converts an int pixel color (type int) to an RGBColor type*/
RGBColor convertPixelColorToRGB(int pix)
{
    RGBColor color;
    GBufferedImage::getRedGreenBlue(pix, color.red, color.green, color.blue);
    return color;
}

/* TODO: Calculates and returns the maximum difference between two RGB colors*/
int getMaxColorDifference(const RGBColor & cOne, const RGBColor & cTwo)
{
    int deltaRed, deltaGreen, deltaBlue;

    deltaRed = abs(cOne.red - cTwo.red);
    deltaGreen = abs(cOne.green - cTwo.green);
    deltaBlue = abs(cOne.blue - cTwo.blue);

    return max(deltaRed, max(deltaGreen, deltaBlue));
}

/* TODO: gets max difference arround neighbors*/
int getMaxNeighborDifference(int row, int col, Grid<int> & imageGrid)
{
    RGBColor center = convertPixelColorToRGB(imageGrid[row][col]);
    int maxDifference = 0;
    for(int i = row-1; i <= row+1; i++){
        for(int j = col-1; j <= col+1; j++){
            if(imageGrid.inBounds(i, j)){
                RGBColor neighbor = convertPixelColorToRGB(imageGrid[i][j]);
                int max = getMaxColorDifference(center, neighbor);
                if(max > maxDifference) { maxDifference = max; }
            }
        }
    }
    return maxDifference;
}

/************ GREEN SCREEN ****************/

/*TODO This function pastes a “sticker” image on top of a “background” image,
 *  but ignores any part of the sticker that is close to pure green in color*/
void greenScreen(GBufferedImage &img)
{
    Grid<int> imageGrid = img.toGrid();
    GBufferedImage sticker;
    getStickerImage(sticker);
    Grid<int> stickerGrid = sticker.toGrid();

    const RGBColor green = convertPixelColorToRGB(GREEN);

    string prompt = "Now choose a tolerance threshold [1-100]: ";
    int threshold = valuePrompt(prompt, 1, 100);
    prompt = "Enter location to place image as \"(row,col)\" (or blank to use mouse): ";
    pixel p = pixelPrompt(prompt);

    for(int r = 0; r < sticker.getHeight(); r++){
        for(int c = 0; c < sticker.getWidth(); c++){
            if(!imageGrid.inBounds(p.row + r,p.col+c)){ break; }
            RGBColor color = convertPixelColorToRGB(stickerGrid[r][c]);
            if(getMaxColorDifference(color, green) > threshold){
                imageGrid[p.row+r][p.col+c] = stickerGrid[r][c];
            }
        }
    }
    img.fromGrid(imageGrid);
}

/*TODO: Function prompts the user to enter a filename for a sticker image*/
void getStickerImage(GBufferedImage &img)
{
    string filename;
    do{
        filename = getLine("Enter a name of image file to open: ");
    }while(!openImageFromFilename(img, filename));
}

/*TODO: prompts the user to enter a location to place 'sticker'.
 * This can be done in two ways: 1) by typing in the "(row,col)" exactly in that format
 * ex (100,150) or 2) by entering "" and then clicking on the background image*/
pixel pixelPrompt(string & prompt)
{
    pixel p;
    do{
        string input = getLine(prompt);
        if(input == ""){
            cout << "Now click the background image to place new image" << endl;
            getMouseClickLocation(p.row, p.col);
            cout << "You chose: (" << p.row << "," << p.col << ")" << endl;
            break;
        }
        try {
            p = stringToPoint(input);
            break;
        }catch(...){}
    }while(true);

    return p;
}

/*TODO: Converts a string input to a pixel type
 * Ex "(10,15)" -> p.row = 10 & p.col = 15*/
pixel stringToPoint(string s)
{
       pixel p;
       Vector<int> numbers;
       TokenScanner scanner(s);
       while(scanner.hasMoreTokens()){
           string token = scanner.nextToken();
           TokenType type = scanner.getTokenType(token);
           if(type == NUMBER){
                numbers.push_back(stringToInteger(token));
           }
       }
       if(numbers.size() == 2){
           string pixelStr = "(" + integerToString(numbers[0]) + "," + integerToString(numbers[1]) + ")";
           if (s == pixelStr) {
               p = {numbers[0], numbers[1]};
               return p;
           }
       }
       error("stringToPoint: Illegal input format");
       return p;
}

/************ ROTATION ****************/

/*TODO: Rotates input image by a given angle
 * between -360 and 360 degrees*/
void rotation(GBufferedImage &img)
{
    Grid<int> imageGrid = img.toGrid();
    Grid<int> rotatedImage(img.getHeight(), img.getWidth(), WHITE);

    string prompt = "Please enter an angle in degrees: ";
    int angle = valuePrompt(prompt, -360, 360);

    for(int r = 0; r < img.getHeight(); r++){
        for(int c = 0; c < img.getWidth(); c++){
            coordinate oldc = rotateCoordinate(pixelToCoordinate((pixel){r, c}, img) , angle);
            pixel oldp = coordinateToPoint(oldc, img);
            if(imageGrid.inBounds(oldp.row, oldp.col)){
                rotatedImage[r][c] = imageGrid[oldp.row][oldp.col];
            }
        }
    }
    img.fromGrid(rotatedImage);
}

/*TODO: Rotates coordinate by input angle*/
coordinate rotateCoordinate(coordinate c, int angle)
{
    coordinate original;
    original.x =  c.x * cosDegrees(angle) - c.y * sinDegrees(angle);
    original.y =  c.x * sinDegrees(angle) + c.y * cosDegrees(angle);
    return original;
}

/*TODO: Converts coordinate type to pixel type
 * Exception is thrown if incorrect input is passed*/
pixel coordinateToPoint(coordinate c, GBufferedImage &img)
{
    pixel p;
    p.row = ceil(c.x) + img.getHeight()/2;
    p.col = ceil(c.y) + img.getWidth()/2;
    return p;
}

/*TODO: Converts pixel type to coordinate type
 * Exception is thrown if incorrect input is passed*/
coordinate pixelToCoordinate(pixel p, GBufferedImage &img)
{
    coordinate c;
    c.x = p.row - img.getHeight()/2;
    c.y = p.col - img.getWidth()/2;
    return c;
}

/************ GAUSSIAN BLUR ****************/

/*TODO:  */
void gaussianBlur(GBufferedImage &img)
{
    Grid<int> imageGrid = img.toGrid();
    Grid<int> blurredGrid(img.getHeight(), img.getWidth(), 0);
    Grid<int> temp(img.getHeight(), img.getWidth(), 0);

    string prompt = "Enter radius for Gaussian Blur that is greater than 0: ";
    int radius = valuePrompt(prompt, 1, 100000);

    Vector<double> kernel = gaussKernelForRadius(radius);
    int sum;
    /*1st pass over rows*/
    for(int r = 0; r < img.getHeight(); r++){
        for(int c = 0; c < img.getWidth(); c++){
            sum = 0;
            for(int i = -radius; i <= radius; i++){
                if(imageGrid.inBounds(r,c+i)) { sum += getWeightedColor(imageGrid[r][c+i], kernel[i+radius]); }
                else { sum += getWeightedColor(imageGrid[r][c-i], kernel[i+radius]); }
            }
            temp[r][c] = sum;
        }
    }

    /*2nd pass over columns*/
    for(int c = 0; c < img.getWidth(); c++){
        for(int r = 0; r < img.getHeight(); r++){
            sum = 0;
            for(int i = -radius; i <= radius; i++){
                if(imageGrid.inBounds(r+i,c)) { sum += getWeightedColor(temp[r+i][c], kernel[i+radius]); }
                else { sum += getWeightedColor(temp[r-i][c], kernel[i+radius]); }
            }
            blurredGrid[r][c] = sum;
        }
    }

    img.fromGrid(blurredGrid);

}

/*TODO: converts pixel color to rgb and multiplies by a weight*/
int getWeightedColor(int pixel, double weight)
{
    RGBColor color = convertPixelColorToRGB(pixel);
    color.red = (int)(color.red*weight);
    color.green = (int)(color.green*weight);
    color.blue = (int)(color.blue*weight);
    return GBufferedImage::createRgbPixel(color.red, color.green, color.blue);
}

//FUNKY LOOKING STUFF
void gaussianBlurFUNKY(GBufferedImage &img)
{
    Grid<int> imageGrid = img.toGrid();

    string prompt = "Enter radius for Gaussian Blur that is greater than 0: ";
    int radius = valuePrompt(prompt, 1, 100000);

    const Vector <double> kernel = gaussKernelForRadius(radius);
    double sum = 0;
    /*1st pass over rows*/
    for(int r = 0; r < img.getHeight(); r++){
        for(int c = 0; c < img.getWidth(); c++){
            for(int i = -radius; i <= radius; i++){
                if(imageGrid.inBounds(r+i,c))
                sum += imageGrid[r+i][c]*kernel[i+radius];
            }
            imageGrid[r][c] = sum;
            sum = 0;
        }
    }

    sum = 0;
    /*2nd pass over columns*/
    for(int c = 0; c < img.getWidth(); c++){
        for(int r = 0; r < img.getHeight(); r++){
            for(int i = -radius; i <= radius; i++){
                if(imageGrid.inBounds(r,c+i)){
                    sum += imageGrid[r][c+i]*kernel[i+radius];
                }
                imageGrid[r][c] = sum;
                sum = 0;
            }
        }
    }

    img.fromGrid(imageGrid);

}

/* Prompts the user to enter a number between a lowerBound and upperBound (inclusive) specified
 Function returns the user's choice if a correct input is given, otherwise user is reprompted */
int valuePrompt(string & prompt, int lowerBound, int upperBound)
{
    int choice;
    do{
        choice = getInteger(prompt, "\nINPUT ERROR: " + prompt);
    }while(choice < lowerBound || choice > upperBound);
    return choice;
}

/* STARTER CODE HELPER FUNCTION - DO NOT EDIT
 *
 * Attempts to open the image file 'filename'.
 *
 * This function returns true when the image file was successfully
 * opened and the 'img' object now contains that image, otherwise it
 * returns false.
 */
bool openImageFromFilename(GBufferedImage& img, string filename) {
    try { img.load(filename); }
    catch (...) { return false; }
    return true;
}

/* STARTER CODE HELPER FUNCTION - DO NOT EDIT
 *
 * Attempts to save the image file to 'filename'.
 *
 * This function returns true when the image was successfully saved
 * to the file specified, otherwise it returns false.
 */
bool saveImageToFilename(const GBufferedImage &img, string filename) {
    try { img.save(filename); }
    catch (...) { return false; }
    return true;
}

/* STARTER CODE HELPER FUNCTION - DO NOT EDIT
 *
 * Waits for a mouse click in the GWindow and reports click location.
 *
 * When this function returns, row and col are set to the row and
 * column where a mouse click was detected.
 */
void getMouseClickLocation(int &row, int &col) {
    GMouseEvent me;
    do {
        me = getNextEvent(MOUSE_EVENT);
    } while (me.getEventType() != MOUSE_CLICKED);
    row = me.getY();
    col = me.getX();
}

/* HELPER FUNCTION
 *
 * This is a helper function for the Gaussian blur option.
 *
 * The function takes a radius and computes a 1-dimensional Gaussian blur kernel
 * with that radius. The 1-dimensional kernel can be applied to a
 * 2-dimensional image in two separate passes: first pass goes over
 * each row and does the horizontal convolutions, second pass goes
 * over each column and does the vertical convolutions. This is more
 * efficient than creating a 2-dimensional kernel and applying it in
 * one convolution pass.
 *
 * This code is based on the C# code posted by Stack Overflow user
 * "Cecil has a name" at this link:
 * http://stackoverflow.com/questions/1696113/how-do-i-gaussian-blur-an-image-without-using-any-in-built-gaussian-functions
 *
 */
Vector<double> gaussKernelForRadius(int radius) {
    if (radius < 1) {
        Vector<double> empty;
        return empty;
    }
    Vector<double> kernel(radius * 2 + 1);
    double magic1 = 1.0 / (2.0 * radius * radius);
    double magic2 = 1.0 / (sqrt(2.0 * PI) * radius);
    int r = -radius;
    double div = 0.0;
    for (int i = 0; i < kernel.size(); i++) {
        double x = r * r;
        kernel[i] = magic2 * exp(-x * magic1);
        r++;
        div += kernel[i];
    }
    for (int i = 0; i < kernel.size(); i++) {
        kernel[i] /= div;
    }
    return kernel;
}
