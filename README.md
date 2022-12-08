# KaitScript

A custom, interpreted programming language.

## Running

The interpreter runs on files with the `.kait` file extension.

To run the interpreter, type:
`kait [file] <options>`
or
`kait -h`

## Syntax

The syntax for KaitScript is modeled off languages like Python, but with typing and bracket scoping for functions and loops.

### Variable Declaration

There are two variable types: `num` and `text` (aka string).
Declare a variable:
```
num pi = 3.1415
text myString = "Hello World!"
```

### Conditional

Conditional statements can be made with the `if`, `elif`, and `else` keywords.

Conditional operators for numbers include `==`, `!=`, `>`, `>=`, `<`, `<=`.
```
num a = 1
num b = 2

num c = 0
if a > b {
    c = a
} elif a == b {
    c = a + b
} else {
    c = b
}
```

Conditional operators for text includes `==`, `!=`.
```
text a = "alice"
text b = "bob"
text c = ""

if a == b {
    c = a
}
if a != b {
    c = b
}
```

### Arrays

Only 1-dimensinal arrays are allowed.
Declare an array:
```
num numbers[] = [1, 2, 3, 4]
text names[] = ["bob", "alice"]
```

### Loops

Loop a defined number of times
```
num a = 0
loop i from 1 to 5 {
    a = a + i
}
```
Infinite loop that must be terminated by a `break`
```
num a = 0
loop i {
    a = a + i
    if a > 20 {
        break
    }
}
```
The variable `i` can have any name and does not need to be defined beforehand. It is implicitly of type `num`.

### Functions

Functions can have return types of `num`, `text`, `num[]`, `text[]`, or none.

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

### Built-in Functions
#### I/O Functions
```
print("Hello world!")
text response = input("Enter input > ")
```
#### Type Conversions
```
num n = int("123")
text t = str(2 * 4)
```

### Comments

Use a `#` for single line comments.
Usage:
```
# Comment
num a = 1

# Another comment
num b = 2
num c = 3
```