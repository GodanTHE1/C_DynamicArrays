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
### Resizing
- char resizeDAf(struct DA* DA,size_t nlen,float bias);  
&nbsp;&nbsp;&nbsp;Resizes the alloced space for the DA bias shifts the arr to one side or other. using nlen = 0 shrinks to len not dealloc.
- char resizeDAI(struct DA* DA,size_t nlen,size_t startoff);  
&nbsp;&nbsp;&nbsp;Resizes the alloced space for the DA start off sifts the start of arr in items. using nlen = 0 shrinks to len not dealloc.
### Core Operations



if you want to just append and prepend set DA->len to 0. if you know what size you will need total you can use the char resizeDAf(struct DA* DA,size_t nlen,float bias) or char resizeDAi(struct DA* DA,size_t nlen,int startoff). these allow you to adjust the size of allocated memory and bias allows you to scew the space to reduce momvment. if you want to save memory putting an nlen of 0 will resize the memory to fit len not dealloc. it is heavly recomended to resize because default growth is only 8 items witch can make raw app or pre very costly O(n^2)~. on failure does not destroy the data in the DA
```c
resizeDAf(DA,250,0.5);//make alloced area 252 long and center current arr
resizeDAi(DA,333,20);//make alloced area 336 long and offset arr by 20 from start
```
all resizeing and inits align Tarr to 4 for convenence 

# error handling 
all functions that return pointers return NULL on failure.
all functions that return char return non zero for errors. the return value can be decoded to find the error.
