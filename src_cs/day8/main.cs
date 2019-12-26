using System;
//using System.Collections.Generic;  // for lists
using System.IO;  // for files
using System.Drawing;  // for bitmap
using System.Drawing.Imaging;  // for image format


public class Program 
{
    public static void Main(string[] args) {
        ImageTranslator trans = new ImageTranslator(25, 6);
        trans.FillFromFile("input");
        
        int layer = trans.GetFewestZerosLayer();

        Console.WriteLine("output = {0}", trans.GetNumberOf(1, layer) * trans.GetNumberOf(2, layer));
        
        string filename = "password.png";
        trans.DisplayImage(filename);
        Console.WriteLine("Image has been written to {0}", filename);
    } 
}


public class ImageTranslator {

    private int _width;
    private int _height;
    private int _layerSize;
    private int _layerCount = -1;  // undefined

    private bool _layersAlgned = false;

    private int[,] _image; 
    private string[] _layerArray;  // layers
    public ImageTranslator(int width, int height) {
        _width = width;
        _height = height;

        // zero out _image (or two it out!)
        _image = new int[width, height];
        for(int i = 0; i < width; i++)
            for(int j = 0; j < height; j++)
                _image[i, j] = 2;  // 2 means transparent
        
        _layerSize = _height * _width;
    }

    //public int GetWidth() { return _width; }
    //public int GetHeight() { return _height; }

    public void FillFromFile(string filename) {
        string line = File.ReadAllLines(filename)[0];
        
        // init how many layers there are
        _layerCount = line.Length / _height / _width; 

        // init image data in readable form. 
        _layerArray = new string[_layerCount];
        for(int i = 0; i < _layerCount; i++) {
            _layerArray[i] = line.Substring(i * _layerSize, _layerSize);
        }
    }

    // This function assumes at least one layer
    public int GetFewestZerosLayer() {
        int min = GetNumberOf(0, 0);
        int index = 0;

        // Check number of zeros in each layer
        for(int i = 1; i < _layerCount; i++) {
            int zeros = GetNumberOf(0, i);
            if (zeros < min) {
                min = zeros;
                index = i;
            }
        }

        // index of min
        return index;
    }

    // Gets the count of <int number> in layer <int layerIndex>
    public int GetNumberOf(int number, int layerIndex) {
        int counter = 0;
        foreach(char ch in _layerArray[layerIndex])  
            if( ch == (char)(number + '0') ) counter++;

        return counter;
    }

    // This function fills the _image multidimentional array
    private void AlignLayers() {
        for(int y = 0; y < _height; y++) {
            for(int x = 0; x < _width; x++) {
                foreach(string layer in _layerArray) {
                    int i = _width * y + x;
                    int val = (int)(layer[i] - '0');
                    
                    if(val != 2) {
                        _image[x, y] = val;
                        break;  // exit condition.    
                    }
                }
            }
        }
    }

    public void DisplayImage(string filename) {
        if(!_layersAlgned) AlignLayers();
        // Todo: validate filename using library function

        // Create and fill bitmap with image data.
        Bitmap bmp = new Bitmap(_width, _height); 
        for(int y = 0; y < _height; y++) {
            for(int x = 0; x < _width; x++) {  
                Color color = color = Color.Transparent;
                switch(_image[x, y]) {
                    case 0: color = Color.White; break;
                    case 1: color = Color.Black; break;
                }

                bmp.SetPixel(x, y, color); 
            }
        }

        bmp.Save(filename, ImageFormat.Png); 
    }
}
