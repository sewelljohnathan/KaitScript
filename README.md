# Kait

A custom, interpreted programming language.

## Running

The interpreter runs on files with the `.kait` file extension.

To run the interpreter, type:
`kait <options> file.kait`
or
`kait -h`

## Syntax

The syntax for Kait is modeled off languages like Python, but with typing and bracket scoping for functions and loops

### Variable Declaration

There are two variable types: `num` and `text` (aka string)
Declare a variable:
```
num pi = 3.1415
text myString = "Hello World!"
```

### Arrays

Only 1-dimensinal arrays are allowed
Declare an array:
```
num numbers[] = [1, 2, 3, 4]
text names[] = ["bob", "alice"]
```

### Loops

```
num a = 0
loop i from 1 to 5 {
    a = a + i
}
```
The variable `i` can have any name and does not need to be defined beforehand. It is implicitly of type `num`

### Functions

Functions can have return types of `num`, `text`, `num[]`, `text[]`, or none

No return type:
```
num a = 1
function myFunc(num n) {
    a = a + n
}
```

Number return type:
```
function triple(num n) num {
    return n * 3
}
num a = myFunc(4)
```

Text array return type:
```
function createNames() text[] {

    text names[] = ["bob", "alice"]
    return names
}
```

### Comments

Use a `#` for single line comments
Usage:
```
# Comment
num a = 1

# Another comment
num b = 2
num c = 3
```