#!/bin/bash

if [ $# -lt 1 ]
then
  echo "What do you think you're doing? I expect:"
  echo " "
  echo " 1nd argument: path where CppUTest is unzipped."
  echo " "
  echo "..what's wrong with you?"
  exit -1
fi

pushd $1

echo "Compiling CppUTest..."

make 
make -f Makefile_CppUTestExt

popd

echo "Compiling completed."
echo "Copying files..."

mkdir include
mkdir lib

cp -r $1/include/* ./include/
cp $1/lib/libCppUTest.a ./lib
cp $1/lib/libCppUTestExt.a ./lib

echo "Copying completed."
