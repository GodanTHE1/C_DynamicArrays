#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

#ifundef DADT
#define DADT int32_t
#end

struct DA {
	size_t Tlen;//align with 4 items
	size_t len;
	DADT* Tarr;
	DADT* arr;
	size_t arr_offset;
};

char initDA(struct DA* nDA,size_t len);
struct DA* NewDA(const size_t  len);
void freeDA(struct DA* DA);
struct DA* copyDA(struct DA* org);

char appDA(struct DA* DA,DADT val);
char preDA(struct DA* DA,DADT val);

char popendDA(struct DA* DA);
char popstartDA(struct DA* DA);

char resizeDAf(struct DA* DA,size_t nlen,float bias);//put back in Tlen for recentering bias of .5 is center
char resizeDAI(struct DA* DA,size_t nlen,size_t startoff);//put back in len for shrinking 


char batchAppDA(struct DA* DA,DADT* vals,size_t vlen);
char batchPreDA(struct DA* DA,DADT* vals,size_t vlen);

char insertDA(struct DA* DA,DADT val,size_t index);
char eraseDA(struct DA* DA,size_t index);

//TODO
char batchInsertDA(struct DA* DA,DADT* vals,size_t* indexs,size_t vlen);
char batchEraseDA(struct DA* DA,size_t* indexs,size_t vlen);

//----function Definitions-------------------------------------------------------------------------------------------------------------------------------------------------
char batchEraseDA(struct DA* DA,size_t* indexs,size_t vlen)
{

	return 0;
}

char batchInsertDA(struct DA* DA,DADT* vals,size_t* indexs,size_t vlen)
{

	return 0;
}



char initDA(struct DA* nDA,size_t len)
{
	if(nDA == NULL) return 0xFF;
	nDA->len = len;
	nDA->Tlen = ((len / 4) + 2) * 4;
	nDA->Tarr = malloc(sizeof(DADT) * nDA->Tlen);
	if(nDA->Tarr == NULL) {
		nDA->arr = NULL;
		return 1;
	}
	nDA->arr_offset = (nDA->Tlen - len)/2;
	nDA->arr = nDA->Tarr + nDA->arr_offset;

	return 0;
}

struct DA* NewDA(const size_t len)
{
	struct DA* nDA = malloc(sizeof(struct DA));
	if(nDA == NULL) return NULL;
	nDA->len = len;
	nDA->Tlen = ((len / 4) + 2) * 4;

	nDA->Tarr = malloc(sizeof(DADT) * nDA->Tlen);
	if(nDA->Tarr == NULL) {
		free(nDA);
		return NULL;
	}
	nDA->arr_offset = (nDA->Tlen - len)/2;
	nDA->arr = nDA->Tarr + nDA->arr_offset;

	return nDA;
}

void freeDA(struct DA* DA)
{
	if(DA == NULL) return;
	free(DA->Tarr);
	free(DA);
}

char appDA(struct DA* DA,DADT val)
{
	if(DA == NULL || DA->Tarr == NULL) return 0xFF;
	if(DA->len + DA->arr_offset < DA->Tlen) {
		DA->arr[DA->len] = val;
		DA->len++;
		return 0;
	}
	if(DA->arr_offset > 1) {
		//move arr back a few update offset
		memmove(DA->Tarr + (DA->arr_offset /= 2),DA->arr,DA->len * sizeof(DADT));
		DA->arr = DA->Tarr + DA->arr_offset;
		DA->arr[DA->len] = val;
		DA->len++;
		return 0;
	}
	//realloc Tarr then copy arr to new location
	DADT* p = realloc(DA->Tarr,(DA->Tlen + 8) * sizeof(DADT));
	if(p == NULL) return 1;
	DA->Tarr = p;
	DA->Tlen += 8;
	memmove(DA->Tarr + (DA->Tlen - DA->len)/2,DA->Tarr + DA->arr_offset,DA->len * sizeof(DADT));
	DA->arr_offset = (DA->Tlen - DA->len)/2; 
	DA->arr = DA->Tarr + DA->arr_offset;
	DA->arr[DA->len] = val;
	DA->len++;
	return 0;
}

char preDA(struct DA* DA,DADT val)
{
	if(DA == NULL || DA->Tarr == NULL) return 0xFF;
	if(DA->arr_offset > 0) {
		DA->arr--;
		DA->arr[0] = val;
		DA->len++;
		DA->arr_offset--;
		return 0;
	}
	if((DA->Tlen - DA->len) > 1) {
		//move arr then set val
		memmove(DA->Tarr + (DA->Tlen - DA->len)/2,DA->arr,DA->len * sizeof(DADT));
		DA->arr_offset = (DA->Tlen - DA->len)/2;
		DA->arr = DA->Tarr + DA->arr_offset;
		DA->arr--;
		DA->arr[0] = val;
		DA->len++;
		DA->arr_offset--;
		return 0;
	}
	//realloc Tarr then copy arr to new location
	DADT* p = realloc(DA->Tarr,(DA->Tlen + 8) * sizeof(DADT));
	if(p == NULL) return 1;
	DA->Tarr = p;
	DA->Tlen += 8;
	memmove(DA->Tarr + (DA->Tlen - DA->len)/2,DA->Tarr + DA->arr_offset,DA->len * sizeof(DADT));
	DA->arr_offset = (DA->Tlen - DA->len)/2; 
	DA->arr = DA->Tarr + DA->arr_offset;
	DA->arr--;
	DA->arr[0] = val;
	DA->len++;
	DA->arr_offset--;
	return 0;
}

char resizeDAf(struct DA* DA,size_t nlen,float bias)
{
	if(DA == NULL || DA->Tarr == NULL) return 0xFF;
	nlen +=  (-nlen) & 3;//make sure nlen is 4 item aligned
	if(nlen < DA->len + (-DA->len & 3)) nlen = DA->len + (-DA->len & 3);//make sure we don't destroy arr by making nlen = len + 4

	// if with the offset bias arr will fit in the current buffer will premove and resize
	if(bias > 1.0) bias = 1.0;if(bias < 0.0) bias = 0;
	size_t noffset = ((nlen - DA->len) * bias);
	if(nlen <= DA->Tlen || noffset + DA->len <= DA->Tlen) {
		//as long as realloc sucseeds data will be in correct position and won't need updating
		memmove(DA->Tarr + noffset,DA->arr,DA->len * sizeof(DADT));
		DADT* p = realloc(DA->Tarr,nlen * sizeof(DADT));
		if(p == NULL) {
			//move data back
			memmove(DA->arr,DA->Tarr + noffset,DA->len * sizeof(DADT));
		   	return 1;
		}
		DA->Tarr = p;
		DA->Tlen = nlen;
		DA->arr_offset = noffset;
		DA->arr = DA->Tarr + DA->arr_offset;
		return 0;
	}
	//arr won't be cut off by resize bc previous if
	DADT* p = realloc(DA->Tarr,nlen * sizeof(DADT));
	if(p == NULL) return 1;
	DA->Tarr = p;
	DA->Tlen = nlen;
	memmove(DA->Tarr + noffset,DA->Tarr + DA->arr_offset,DA->len * sizeof(DADT));
	DA->arr_offset = noffset;
	DA->arr = DA->Tarr + DA->arr_offset;
	return 0;
}

struct DA* copyDA(struct DA* org)
{
	if(org == NULL || (org->Tarr == NULL && org->Tlen != 0)) return NULL;

	struct DA* new = malloc(sizeof(struct DA));
	if(new == NULL) return NULL;

	if(org->Tarr == NULL) {
		new->Tarr = NULL;
		new->Tlen = 0;
		new->arr = NULL;
		new->len = 0;
		new->arr_offset = 0;
		return new;
	}
	new->Tarr = malloc(org->Tlen * sizeof(DADT));
	if(new->Tarr == NULL) {
		free(new);
		return NULL;
	}
	new->Tlen = org->Tlen;

	if(org->arr == NULL) {
		new->arr = NULL;
		new->len = 0;
		new->arr_offset = 0;
		return new;
	} else {
		new->arr_offset = (org->arr - org->Tarr);
		new->arr = new->Tarr + new->arr_offset;
		new->len = org->len;
	}
	memmove(new->arr,org->arr,new->len * sizeof(DADT));
	return new;
}

char popstartDA(struct DA* DA)
{
	if(DA == NULL || DA->Tarr == NULL) return 0xFF;
	if(DA->len == 0) return 0;
	DA->arr++;
	DA->arr_offset++;
	DA->len--;
	return 0;
}

char popendDA(struct DA* DA)
{
	if(DA == NULL || DA->Tarr == NULL) return 0xFF;
	if(DA->len == 0) return 0;
	DA->len--;
	return 0;
}

char eraseDA(struct DA* DA,size_t index)
{
	if(DA == NULL || DA->Tarr == NULL) return 0xFF;
	if(DA->len <= 0) return 0;
	if(DA->len <= index ) return 1;
	
	if(2 * DA->arr_offset >= DA->Tlen - DA->len) {
		//more space in offset so increse end space
		DA->len--;
		memmove(DA->arr + index,DA->arr + index + 1,(DA->len - index) * sizeof(DADT));
	} else {
		//more space in end so increse offset space
		DA->len--;
		memmove(DA->arr + 1,DA->arr,(index) * sizeof(DADT));
		DA->arr++;
		DA->arr_offset++;
	}
	return 0;
}

char insertDA(struct DA* DA,DADT val,size_t index)
{
	if(DA == NULL || DA->Tarr == NULL) return 0xFF;
	if(index >= DA->len) {
		char res = appDA(DA,val);
		if(res) return res;
		return 0;
	}
	if(DA->arr_offset == 0 && DA->len >= DA->Tlen) {
		char res = resizeDAf(DA,DA->Tlen + 8, 0.5);//makes space for insert
		if(res) return res;
	}
	if(2 * DA->arr_offset >= DA->Tlen - DA->len) {
		//more space in offset/front
		memmove(DA->arr - 1,DA->arr,index * sizeof(DADT));
		DA->arr[index] = val;
		DA->arr--;
		DA->arr_offset--;
		DA->len++;
	} else {
		//more space in end
		memmove(DA->arr + index + 1,DA->arr + index,(DA->len - index) * sizeof(DADT));
		DA->arr[index] = val;
		DA->len++;
	}
	return 0;
}

char batchPreDA(struct DA* DA,DADT* vals,size_t vlen)
{
	if(vlen == 0) return 0;
	if(DA == NULL || vals == NULL) return 0xFF;
	ptrdiff_t vself = 0;char vflag = 0;
	if(vals >= DA->Tarr && vals < DA->Tarr + DA->Tlen) {vself = vals - DA->Tarr;vflag = 1;}
	if(DA->arr_offset >= vlen) {
		//if there is space just appends items
		DA->arr_offset -= vlen;
		DA->arr -= vlen;
		memmove(DA->arr, vals, vlen*sizeof(DADT));
		DA->len += vlen;
		return 0;
	}
	if(DA->Tlen < DA->len + vlen) {
		//resizes to fit new data
		size_t nlen = DA->len + vlen;
		nlen += (-nlen & 3);//align to 4 iteams
		DADT* p = realloc(DA->Tarr,nlen * sizeof(DADT));
		if(p == NULL) return 1;
		DA->Tarr = p;
		DA->arr = DA->Tarr + DA->arr_offset;
		DA->Tlen = nlen;
	}
 	//moves memory so items can be added	
	size_t noffset = (DA->Tlen - (DA->len + vlen))/2;
	memmove(DA->Tarr + noffset + vlen,DA->arr,DA->len * sizeof(DADT));
	if(vflag) vals = DA->Tarr + vself + noffset + vlen - DA->arr_offset;
	DA->arr_offset = noffset;
	DA->arr = DA->Tarr + noffset;

	//use memmove to set memory 
	memmove(DA->arr,vals,vlen * sizeof(DADT));
	DA->len += vlen;
	return 0;
}

char batchAppDA(struct DA* DA,DADT* vals,size_t vlen)
{
	if(vlen == 0) return 0;
	if(DA == NULL || vals == NULL) return 0xFF;
	//if there is enough space to just insert will insert
	if(DA->Tlen >= DA->arr_offset + DA->len + vlen) {
		memmove(DA->arr + DA->len,vals,vlen * sizeof(DADT));
		DA->len += vlen;
		return 0;
	}
	ptrdiff_t vself = 0;char vflag = 0;
	if(vals >= DA->Tarr && vals < DA->Tarr + DA->Tlen) {vself = vals - DA->Tarr;vflag = 1;}
	if(DA->Tlen < DA->len + vlen) {
		//resizes to fit new data
		size_t nlen = DA->len + vlen;
		nlen += (-nlen & 3);//align to 4 iteams
		DADT* p = realloc(DA->Tarr,nlen * sizeof(DADT));
		if(p == NULL) return 1;
		DA->Tarr = p;
		DA->arr = DA->Tarr + DA->arr_offset;
		DA->Tlen = nlen;
	} 

	//moves arr to fit appended data
	size_t noffset = (DA->Tlen - (DA->len + vlen))/2;
	memmove(DA->Tarr + noffset,DA->arr,DA->len * sizeof(DADT));
	if(vflag) vals = DA->Tarr + vself - DA->arr_offset + noffset;
	DA->arr_offset = noffset;
	DA->arr = DA->Tarr + DA->arr_offset;
	
	
	//use memmove to set memory because I can't remember memcpy sytax
	memmove(DA->arr + DA->len,vals,vlen * sizeof(DADT));
	DA->len += vlen;
	return 0;
}

char resizeDAI(struct DA* DA,size_t nlen,size_t startoff)//put back in len for shrinking
{
	if(DA == NULL || DA->Tarr == NULL) return 0xFF;
	
	//make sure startoff fits in nlen if not make nlen fit
	if(nlen < DA->len + startoff) nlen = DA->len + startoff;

	nlen +=  (-nlen) & 3;//make sure nlen is 4 item aligned
	if(nlen < DA->len + (-DA->len & 3)) nlen = DA->len + (-DA->len & 3);//make sure we don't destroy arr by making nlen = len + 4

	size_t noffset = startoff;//why rewrite the old var 
	if(nlen <= DA->Tlen || noffset + DA->len <= DA->Tlen) {
		//as long as realloc sucseeds data will be in correct position and won't need updating
		memmove(DA->Tarr + noffset,DA->arr,DA->len * sizeof(DADT));
		DADT* p = realloc(DA->Tarr,nlen * sizeof(DADT));
		if(p == NULL) {
			//move data back
			memmove(DA->arr,DA->Tarr + noffset,DA->len * sizeof(DADT));
		   	return 1;
		}
		DA->Tarr = p;
		DA->Tlen = nlen;
		DA->arr_offset = noffset;
		DA->arr = DA->Tarr + DA->arr_offset;
		return 0;
	}
	//arr won't be cut off by resize bc previous if
	DADT* p = realloc(DA->Tarr,nlen * sizeof(DADT));
	if(p == NULL) return 1;
	DA->Tarr = p;
	DA->Tlen = nlen;
	memmove(DA->Tarr + noffset,DA->Tarr + DA->arr_offset,DA->len * sizeof(DADT));
	DA->arr_offset = noffset;
	DA->arr = DA->Tarr + DA->arr_offset;
	return 0;
}
