# C_DynamicArrays
This is my simple dynamic Array implmentation in C. This implementation focuses on **keeping memory usage tight** while offering flexible resizing and insertion options.

## Features
- Append, prepend, insert, and erase operations
- Batch operations for better performance
- Flexible resizing with bias/offset control
- Macro-based generic design

## Example setup
```c
#define DADT int
...
struct DA* DA = newDA(10);//makes new DA have arr as len 10
for(int i = 0;i < 10;i++) DA->arr[i] = i;//fill arr with vals
appDA(333); //append
preDA(-999);//prepend
printf("\nDA 10: %i\n",DA->arr[11]);
freeDA(DA);
```
## function Ref
### Creation and Destruction 
- char initDA(struct DA* nDA,size_t len);  
&nbsp;&nbsp;&nbsp;Used to init Stack allocked DA's !! Rember to manually free "DA.Tarr" when done
- struct DA* NewDA(const size_t  len);  
&nbsp;&nbsp;&nbsp;Allocs a new DA
- void freeDA(struct DA* DA);  
&nbsp;&nbsp;&nbsp;Frees a heap alloced DA !! Do not use on stack DA
- struct DA* copyDA(struct DA* org);  
&nbsp;&nbsp;&nbsp;Returns a pointer to a copy with copied arr contents
### Resizing
- char resizeDAf(struct DA* DA,size_t nlen,float bias);  
&nbsp;&nbsp;&nbsp;Resizes the alloced space for the DA bias shifts the arr to one side or other. using nlen = 0 shrinks to len not dealloc.
- char resizeDAI(struct DA* DA,size_t nlen,size_t startoff);  
&nbsp;&nbsp;&nbsp;Resizes the alloced space for the DA start off sifts the start of arr in items. using nlen = 0 shrinks to len not dealloc.
### Core Operations
- char appDA(struct DA* DA,DADT val);<br>
&nbsp;&nbsp;&nbsp;Appends to the end of arr. If no space shifts, if no space to shift reallocs.
- char preDA(struct DA* DA,DADT val);<br>
&nbsp;&nbsp;&nbsp;Prepends to the start of arr. If no space shifts, if no space to shift reallocs.
- char popendDA(struct DA* DA);<br>
&nbsp;&nbsp;&nbsp;Removes the last element, just len - 1.
- char popstartDA(struct DA* DA);<br>
&nbsp;&nbsp;&nbsp;Removes the first element, arr++ len - 1;
- char insertDA(struct DA* DA,DADT val,size_t index);<br>
&nbsp;&nbsp;&nbsp;Inserts A item at index shifting the list to make space, will do slow Shifts to avoid reallocs.
- char eraseDA(struct DA* DA,size_t index);<br>
&nbsp;&nbsp;&nbsp;Removes A item at index shifting the list to cover space.
### Batch Operations
- char batchAppDA(struct DA* DA,DADT* vals,size_t vlen);<br>
&nbsp;&nbsp;&nbsp;Appends a whole array of vals to the end of arr
- char batchPreDA(struct DA* DA,DADT* vals,size_t vlen);<br>
&nbsp;&nbsp;&nbsp;Prepends a whole array of vals to the start of arr
- char batchInsertDA(struct DA* DA,DADT* vals,size_t* indexs,size_t vlen);<br>
&nbsp;&nbsp;&nbsp;Inserts A Batch of Values into Arr, indexs are calculated before insertion.
- char batchEraseDA(struct DA* DA,size_t* indexs,size_t vlen);<br>
&nbsp;&nbsp;&nbsp;Removes A Batch of Values from Arr, indexes are calculated before removal.
### Struct Ref "struct DA"
- DADT - the macro witch Defines the type arr is.
- Tarr - the start of allocated memory alligned 4 items
- arr - the array
- Tlen - the lengnth of Tarr aligned 4 items
- len - the legnth of arr


```c
resizeDAf(DA,250,0.5);//make alloced area 252 long and center current arr
resizeDAi(DA,333,20);//make alloced area 336 long and offset arr by 20 from start
```
all resizeing and inits align Tarr to 4 for convenence 

# error handling 
all functions that return pointers return NULL on failure.
all functions that return char return non zero for errors. the return value can be decoded to find the error.
