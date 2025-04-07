# Expression Parser

This project implements an expression parser using C++23. It includes a Lexer, Parser, and manual test executables for testing expressions.

## Building the Project

1. Open a terminal at the project root.
2. Create and navigate to a build directory:

   ```sh
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

This will create the test executables `testLexer` and `testParser` in the build directory.

## Running Tests Manually

The test executables require a file path as an argument.Please, note that not all expressions will work with the lexer, since it's testing is more limited. To run the tests using the provided example file, execute the following commands from the build directory:

   ```sh
   ./testLexer ../tests/data/examples.js
   ./testParser ../tests/data/examples.js
   ```

## Testing Individual Expressions

The file `tests/data/examples.js` contains several expressions. To test a specific expression:

1. Open `tests/data/examples.js` in a text editor.
2. Comment out or uncomment lines so that only the expression you want to test remains active.
3. Save your changes.
4. Run the test executables again using the commands above.

