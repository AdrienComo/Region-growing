# General Program for Region Growing

This project has always kept in mind that large images take a lot of time to process. 
We have therefore always kept the code as simple and clean as possible to maximize the speed at which the segmentation is done. This allows us to
provide a low cost, immediate results, segmentation solution.

Detailed explanations of how the program works can be found in ```file.pdf```. <br>
Performance and data structures discussion can be found in the same file. <br>

### How to run

At the root of the project type :
```make clean && make && clear && ./bin/main a b c d e f g```<br> <br>
- a : input path to image (img/…) (works with png, jpg, jpeg) <br>
- b : output path (img/…) (will write new image in .png)<br>
- c : 0 < x < 20 : Gaussian blur kernel size<br>
- d : number of seeds (1 < x < 4,294,967,295)<br>
- e : options (1 : average color of region, 2 : random color)<br>
- f : threshold range (0-255)<br>
- g : second threshold range(0-255)<br>
- h : fusion threshold between region range(0-255)<br>
- i : min for contour (0<x) <br>
- j : max for contour (min<x) <br>

The higher the thresholds, the more agressively will the region be fused/grow large. <br><br>

### Output

![Regions](img/landscape_1.jpg?raw=true "Regions")
![Borders](img/borders_1.jpg?raw=true "Borders") <br>

> We can precisely see the regions on the first image and all the borders of each regions on the second image. <br>

### Performance

An image of : 600 by 450 pixels can be segmented in around 80 ms (milliseconds). <br>
Much larger images : 1200 by 700 can be segmented on average in 240 ms. <br>
After further tests, we can conclude that this algorithm is capable of sampling **~3.350.600 pixels/seconds** on 16 Gb of RAM and a 6 core CPU. <br>

### Example 
```
make clean && make && clear && ./bin/main img/image_landscape.png output 600 5 1 35 1 10 10 100
```

### Dependencies 
This code uses the OpenCV C++ library.



