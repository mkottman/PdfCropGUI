Overview
=======

PdfCropGUI is a GUI tool to crop PDF files. It is based on [pdfcrop.pl](http://www.ctan.org/tex-archive/support/pdfcrop/), but allows you to enter the desired cropbox visually, straight on the preview of the actual PDF. Comes in handy if you have an ebook reader and want to remove the large margins that many science articles have, so that they fill the screen completely.

Dependencies
--------------------

PdfCropGUI is written using the [Qt](http://qt.nokia.com/) library. It uses [poppler](http://poppler.freedesktop.org/) for displaying PDF previews, more specifically it's Qt front-end. For the actual PDF processing, it uses [LaTeX](http://www.latex-project.org/) (ok, just the `pdflatex` executable actually).

Here is a list of the packages you need in Ubuntu:

* libqt4-dev
* libpoppler-qt4-dev
* texlive-latex-base

Compiling and running
----------------

Open the project file in QtCreator and run it, or run the following commands in the project directory:

    $ qmake
    $ make
    $ ./PdfCropGUI

Usage
---------

The usage is really simple:

1. Open your PDF file through the menu, or Ctrl+L
2. Select a page on which you want to clip, a preview image is shown.
3. Drag a box around the area you are interested in, or enter the values directly in the cropbox field.
4. Press the Crop! button.
5. The progress is shown in a progress bar. When finished, the result is saved in a file called '_SourceFile__cropped.pdf'.
