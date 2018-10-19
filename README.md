## TinyJSON
A lightweight implement of JSON parser in C


#### TODO list
* prepare to add some function which implement some simple operation based on JSON data structure.

#### How to use TinyJSON in my code ?
  Oops, I never say how to make your code run normally, because there is a better way. Let us to read some example code below.
```C
#include "tj/tinyjson.h"
/* if your code and the 'tj' directory are both at the same directory, you can include the "tinyjson.h" header file just like this form */

int main(int argc, char **argv) {

    /* some other code ... */
    
    /* initialize some parameters and malloc memory for parsing, the argument 's' is a pointer to json string (wchar_t *) */
    setParser(s); 
    
    /* parse json string, then generate a top json_node structure which makes up with the whole json data and it's just like a tree */
    json_node *node = parse();
    
    /* print formated json_node data to stdout */
    printNode(node);
    
    /* free the memory which be malloced in setParser() */
    destroyParser();
    
    /* some other code ... */
}

```
