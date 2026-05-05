# C_DynamicArrays
This is my simple dynamic Array implmentation in C. This implmentation focases on keeping memory tight and reducing reallocs.

# Use
to use this libary include like a normal libary. there is two important types and macros: DADT and struct DA. currently you set the type of DA with DADT and that is the type of your DA. the struct DA has 5 feilds the important one for use is "DA.arr" witch is the your array and "DA.len" witch is the legnth of your array. you can use this like a normal arr directly but if you app or prepend items update your pointers. before using the DA struct you want to throw it in either char initDA(struct DA* DA,size_t len) or struct DA* newDA(size_t len). freeDA only works on dynamicall alloced bases for bases on the stack you have to manually free the "DA.Tarr" feild.

Example setup
```c
#define DADT int
...
struct DA* DA = newDA(10);//makes new DA have arr as len 10
for(int i = 0;i < 10;i++) DA->arr[i] = i;//fill arr with vals
appDA(333);//append
preDA(-999);//prepend
printf("\nDA 10: %i\n",DA->arr[11]);
freeDA(DA);
```
if you want to just append and prepend set DA->len to 0. if you know what size you will need total you can use the char resizeDAf(struct DA* DA,size_t nlen,float bias) or char resizeDAi(struct DA* DA,size_t nlen,int startoff). these allow you to adjust the size of allocated memory and bias allows you to scew the space to reduce momvment. if you want to save memory putting an nlen of 0 will resize the memory to fit len not dealloc. it is heavly recomended to resize because default growth is only 8 items witch can make raw app or pre very costly O(n^2)~. on failure does not destroy the data in the DA
```c
resizeDAf(DA,250,0.5);//make alloced area 252 long and center current arr
resizeDAi(DA,333,20);//make alloced area 336 long and offset arr by 20 from start
```
all resizeing and inits align Tarr to 4 for convenence 

# error handling 
all functions that return pointers return NULL on failure.
all functions that return char return non zero for errors. the return value can be decoded to find the error.
