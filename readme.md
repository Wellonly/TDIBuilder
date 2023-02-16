
# The Tree Device Interface builder: TDI builder
The TDI builder is an extension for Qt Creator that make possible of creation and edition
  a high-level communicate interfaces for microcontroller devices.

  As input data it take an executable elf(dwarf2 compliant) file, parse it with help of GCC binutils,
build a data tree of all used static variables and routines with defined signatures, then it used for linking interface items with its data.

  So, a device with TDI can be interacted with the Device explorer or other devices upon it,
thanks to the data driven technology used under the hood.

This is a pet project, for Qt Creator 5.x.

MIT License Copyright (c) 2023 Well <github.com/Wellonly>
