# Functions and Parameters Principles

As a developer coming from the more "comfortable" language PHP, C poses some unexpected challeges 

## Passing / Returning parameter to a function

Coming from PHP, I often see Functions/Methods written something like this 

```
public function main() {
    $myArray = ['a', 'b', 'c'];

    $myArray = doSomethingWithArray($myArray);
}

public function doSomethingWithArray(array $myArray): array
{
    $myArray[] = 'd';
    return $myArray;
}
```

C will not accept something like this. First reason is that functions in C do not accept strings, arrays, objects etc. as input or return parameters. But as a programmer I still may want to outsource some logic into a function or method, to keep code more overviewable, so how would I manipulate my array in C? The answer is using pointers. Something also possible (and more efficient) in PHP. Advantages would be slightly more performance and less RAM usage. 
<br><br>
Thinking in C, what actually happens when PHP excecutes the above code is (mixed up pseudo code that wont work, but tries to explain the problem)
```
public function main() {
    $myArray = ['a', 'b', 'c']; // create original array

    $myNewArrayPointer = doSomethingWithArray(&$myArrayPointer); // send pointer to original array into function
    $myArray = $myNewArrayPointer; //take data from new array and save in original array
}

public function doSomethingWithArray(array $myArrayPointer): arrayPointer
{
    $myNewArray = $myArrayPointer; // create an array using data from the original array referenced by pointer
    $myNewArray[] = 'd'; // manipulate the new array
    return &$myNewArrayPointer; // return pointer to new array
}
```
So doSomethingWithArray creates a copy of the first array, manipulates it and returns a pointer to the copied array. Back in main() we replace the original array with the content of the manipulated array. Wasted resources here. <br><br>
Even in pure correct PHP, this code can be more effective
```
public function main() {
    $myArray = ['a', 'b', 'c'];

    doSomethingWithArray(&$myArray);
}

public function doSomethingWithArray(array $myArray): arrayPointer
{
    $myArray[] = 'd';
}
```
And this is the mindset we need to develop in C<br>
Same code different syntax (not sure if 100% correct, but the mindset is important here)
```
void doSomethingWithArray(char[]* myArray) {
    myArray[3] = 'd';
}

int32_t main(void* p) {
    UNUSED(p); //dont ask me why
    char myArray[4] = ["a", "b", "c"]; // string is also an array
    doSomethingWithArray(&myArray);

    return 0;
}
```

