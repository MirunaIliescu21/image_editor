# Image Editor - `imagine_editor.c`

The `imagine_editor.c` program processes image files, performing various operations at the photo level, similar to a photo editor.

## Data Structures
Before performing operations, two main data structures are declared:
- **Image structure**: Contains all necessary image data including:
  - **Magic parameter** (image type: P2, P3, P5, P6)
  - **Image dimensions**
  - **Maximum pixel value**
  - **Matrices**:
    - **Grayscale matrix** (`int[][]`): Stores grayscale pixels.
    - **Color matrix** (`color_rgb[][]`): Stores color pixels in a structure with three fields for red, green, and blue color channels.

The image structure also includes fields for storing selection coordinates.

## Functions Overview
The program contains several functions, some used exclusively for the required operations and others that are reused throughout. For example, memory allocation and deallocation functions for grayscale and color matrices are reused frequently. These functions use defensive programming to prevent memory errors.

## Supported Operations

### 1. LOAD - Loading an Image
The `load_image` function loads an image into memory:
- The file is opened in text mode.
- The function identifies the image type (P2/P5 for Grayscale or P3/P6 for Color) and opens the file accordingly.
- Text files are read directly, while binary files use `ftell` and `fseek` to retain cursor position, allowing reopening in binary mode.
- Based on image type, memory is allocated, and data is read using `fscanf()` (for text files) or `fread()` (for binary files).
- A helper function removes comments from files, ignoring lines that start with `#`.

### 2. SAVE - Saving Files (ASCII/Binary)
Separate save functions are used for text and binary file formats:
- If the third parameter in the input command is missing, the file is saved in binary format using `fwrite()`.
- If "ascii" is specified as the third parameter, the data is saved in text format with `fprintf()`.
- Color matrix fields (red, green, blue) are read/written individually.
- Files are closed after processing to release resources.

### 3. SELECT - Selecting an Image Portion
There are two selection types: 
- `SELECT ALL`: Selects the entire image.
- `SELECT <x1> <y1> <x2> <y2>`: Selects a smaller portion of the image based on specified coordinates.
- Two functions handle these operations, verifying input parameters and updating selection coordinates if valid.

### 4. CROP - Cropping the Image
Separate crop functions handle grayscale (`crop_grayscale`) and color (`crop_color`) images:
- A dynamically allocated auxiliary matrix copies only the selected portion.
- Memory is reallocated to hold cropped data.
- Differences in color and grayscale cropping methods are based on auxiliary matrix type and memory handling for pixel colors.

### 5. APPLY - Applying Filters
Filters apply only to color images:
- `apply_filter` verifies the filter type and uses the corresponding kernel matrix (3x3 double) for each filter.
- The filter operation multiplies kernel values with each pixel and its neighbors, summing results and rounding to [0,255] with `clamp()`.
- All pixels except those at the edges are affected, as edge pixels lack 8 neighbors.

### 6. HISTOGRAM
The histogram function computes image frequency with x stars and y bins:
- A frequency vector dynamically calculates pixel intensity (0–255).
- After equalizing, grayscale values optimize to enhance human perception.
- The function displays the histogram based on the computed star frequency.

### 7. EQUALIZE
The equalize function adjusts grayscale pixels based on a formula provided in the documentation:
- Pixel frequency is computed and each matrix element is recalculated using the formula, then clamped to the [0,255] range.

### 8. ROTATE
The rotate function rotates the entire image or a selected portion:
- Separate rotation functions handle color and grayscale images for full or partial rotation.
- Rotation angles are restricted to multiples of 90 degrees within [-360,360], meaning a maximum of three 90-degree rotations.
- Dynamic memory allocation in an auxiliary matrix allows efficient rotation.
- After rotation, dimensions are swapped and the auxiliary matrix is copied to the main matrix.

### 9. EXIT - Exiting the Program
The exit function frees all allocated memory and stops program execution. This function is also called in other operations to ensure efficient memory usage by preventing multiple images from occupying memory simultaneously.

---

This program offers an efficient solution for image processing tasks with defensive memory management and precise handling of grayscale and color formats.
