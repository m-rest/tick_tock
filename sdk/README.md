Legacy SDKs go here for historic reference.

## HOWTO

### AAX 

#### Mac OS

- download AAX SDK 2.4.0
- extract .zip 
- build libAAXLibrary_libcpp.a with XCODE project from AAX SDK subfolder. Set RELEASE or DEBUG in "Product->Scheme->Edit Scheme"

- set path in Projucer to main folder of extracted SDK
- build plugin with xcode, automatically linking previously built AAX library
