//------- Limited size vector
#ifndef ART_LIMVEC_H
#define ART_LIMVEC_H

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;

//Description:
//The limited size vector destroy its head date to push new data 
template<typename Type, typename Allocator = std::allocator<Type> >
class ArtLimVector: public vector<Type, Allocator> 
{
typedef vector<Type, Allocator> stlVec;	
typedef ArtLimVector<Type, Allocator> limVec;

public:
	static const long int DEF_LVEC_MAX_SIZE=100;
	
	ArtLimVector ( long int max_size_init = DEF_LVEC_MAX_SIZE, long int init_size = 0 );
	ArtLimVector ( const stlVec &vec_init);
	//ArtLimVector ( const limVec &vec_init);
	void ArtLimVectorDef ();
	
	//Return: new size
	//cut_head: 
	//		if the new size is shorter than the old one:: true - cuts the first bytes ; false - cut the tail bytes;
	//preserve cur size = true - it will not allow to make max size < than the current size
	long int setMaxSize ( long int sz, bool preserve_cur_size = false, bool cut_head = true, Type c = Type () ); 
	//cut_head: 
	//		if the new size is shorter than the old one:: true - cuts the first bytes ; false - cut the tail bytes;
	//		if the new size is large than the old one:: true - add bytes to head; false - add new bytes to tail;
	long int resize ( long int sz, bool cut_head, bool increase_max_size, Type c = Type () );
	void resize ( size_t sz, Type c = Type () );//Behaves as a standart vector resize (i.e. cut_head=false, increase_max_size=true)
	void clear(bool fast_clear = false);//Preserves Max Size

	long int getMaxSize ();
	long int size();
	Type& operator [] (long int index);
	Type& at(long int index);	
	limVec& operator= (const stlVec &obj);
	Type& front();
	Type& back();	
	
	void push_back (const Type& obj);
	void pop_back (bool delete_when_pop = true);//delete_when_pop - substitutes the deleted element with the element by default
	void push_front (const Type& obj);
	void pop_front (bool delete_when_pop = true);//delete_when_pop - substitutes the deleted element with the element by default
	bool empty();

	long int getFirstNum() {return first_element_num;}
	long int getLastNum() {return last_element_num;}
	
//-------Don't need to be reimplemented
	//void reserve (size_t n);
	//get_allocator ()

	
protected:
	//---Parameters
	long int first_element_num;
	long int last_element_num;
	
	//---Ancillary functions
	void check_elem_nums();//Correct them if they are < 0

	
	//----Just hide
	void rbegin(){}
	void rend(){}
	void begin(){}
	void end(){}
	
	void insert(){}
	void erase(){}
	void assign(){}
	void swap(){}
	void max_size(){}
	
	
private:

	//begin ()
	//end ()
	//rbegin ()
	//rend ()
	//capacity ()
	//at
	//insert ()
	//erase ()

};

template< typename Type, typename Allocator  >
inline void ArtLimVector<Type, Allocator>:: ArtLimVectorDef ()
{

	first_element_num = -1;
	last_element_num = -1;
	stlVec::reserve(2);
	setMaxSize(0);
}

template< typename Type, typename Allocator  >
ArtLimVector<Type, Allocator>:: ArtLimVector (long int max_size_init, long int init_size )
{
	ArtLimVectorDef();
	if (max_size_init < 0) max_size_init = 0;
	stlVec::reserve(  (!max_size_init) ?  2: (max_size_init * 1.1 )  );
	setMaxSize(max_size_init);
	resize (init_size);
}


template< typename Type, typename Allocator  >
ArtLimVector<Type, Allocator>:: ArtLimVector (const stlVec &obj )
{
	ArtLimVectorDef();
	*this  = obj;

}


template< typename Type, typename Allocator  >
inline Type& ArtLimVector<Type, Allocator>::front ()
{
return stlVec::at(first_element_num);
}

template< typename Type, typename Allocator  >
inline Type& ArtLimVector<Type, Allocator>::back ()
{
return stlVec::at(last_element_num);
}




template< typename Type, typename Allocator  >
Type& ArtLimVector<Type, Allocator>::operator[] (long int index)
{
	if ( index >= size() ) index = size () - 1;
	if ( index < 0 ) index = 0;
	if ( first_element_num <= last_element_num )  
return stlVec::at( first_element_num + index );
	else
	{	
return stlVec::at( first_element_num + index -  ( ( (first_element_num + index) < stlVec::size()) ? 0 : stlVec::size() )   );			
	}
}

template< typename Type, typename Allocator  >
inline Type& ArtLimVector<Type, Allocator>::at (long int index)
{
return operator[](index);
}



template< typename Type, typename Allocator  >
ArtLimVector<Type, Allocator> & ArtLimVector<Type, Allocator>:: operator= (const stlVec& obj)
{	
	clear();
	if( getMaxSize() < obj.size() ) setMaxSize( obj.size() );
	//cout<<"getMax=" << getMaxSize() << "  obj.size=" << obj.size() << endl;
	const long int sz = obj.size();
	for (int i=0; i< sz; i++)
	{
		push_back( obj[i] );
	}
return *this;	
}






template< typename Type, typename Allocator  >
void ArtLimVector<Type, Allocator>::push_back (const Type& obj)
{
	check_elem_nums();
	if (stlVec::size() == 0)
return;		
	last_element_num++;
	if (last_element_num >= stlVec::size()) last_element_num = 0;
	if (first_element_num == last_element_num) first_element_num++;
	if ((first_element_num >= stlVec::size())  || (first_element_num < 0) ) first_element_num = 0;
	stlVec::at(last_element_num) = obj;
}

template< typename Type, typename Allocator  >
void ArtLimVector<Type, Allocator>::pop_back (bool delete_when_pop)
{
	check_elem_nums();
	if ( (stlVec::size() == 0) || (limVec::size()==0) ) 
return;

	if (delete_when_pop) stlVec::at(last_element_num) = Type();
	if ( first_element_num == last_element_num )
	{
		first_element_num = -1;
		last_element_num = -1;	
	}
	else
	{
		
		if (last_element_num == first_element_num)
		{
			first_element_num = -1;
			last_element_num = -1;
		}
		else 
		{
			last_element_num--;
			if ( last_element_num < 0 ) last_element_num = stlVec::size() - 1;
		}
	}
}

template< typename Type, typename Allocator  >
void ArtLimVector<Type, Allocator>::push_front (const Type& obj)
{
	check_elem_nums();
	if (stlVec::size() == 0)
return;
	first_element_num--;
	if (first_element_num < 0) first_element_num = stlVec::size() - 1;
	if (first_element_num == last_element_num) last_element_num--;
	if (last_element_num < 0) last_element_num = stlVec::size() - 1;
	stlVec::at(first_element_num) = obj;
}

template< typename Type, typename Allocator  >
void ArtLimVector<Type, Allocator>::pop_front (bool delete_when_pop)
{
	check_elem_nums();
	if ( (stlVec::size() == 0) || (limVec::size()==0) ) 
return;
	if (delete_when_pop) stlVec::at(first_element_num) = Type();
	if ( first_element_num == last_element_num )
	{
		first_element_num = -1;
		last_element_num = -1;	
	}
	else
	{
		
		if (last_element_num == first_element_num)
		{
			first_element_num = -1;
			last_element_num = -1;
		}
		else 
		{
			first_element_num++;
			if (first_element_num >= stlVec::size()) first_element_num = 0;
		}
	}

}








template< typename Type, typename Allocator  >
inline bool ArtLimVector<Type, Allocator>:: empty() 
{
return stlVec::size() && limVec::size(); 
}


template< typename Type, typename Allocator  >
long int ArtLimVector<Type, Allocator>:: size () 
{
long int cur_size = 0;
	//cout<<"size called"<<endl;
	check_elem_nums();

	
	if (stlVec::size() == 0)
return 0;		
	
	if ( (first_element_num < 0) || (last_element_num < 0) )
return 0;		
	//cout<<"Calc size"<<endl;

	//if (first_element_num == last_element_num)
//return 1;

	if (first_element_num <= last_element_num)
	{
		cur_size = last_element_num - first_element_num + 1;
	}
	else
	{
		cur_size = stlVec::size() - ( first_element_num - last_element_num ) + 1; 
	}

	if (cur_size < 0) cur_size = 0;
return cur_size;	
}



template< typename Type, typename Allocator  >
long int ArtLimVector<Type, Allocator>::setMaxSize( long int sz, bool preserve_cur_size, bool cut_head, Type c )
{
stlVec * p_tempvec = 0;
	
	check_elem_nums();

	if ( sz < 0 ) sz = 0;
	if ( sz == stlVec::size() )
return stlVec::size();

	if (preserve_cur_size && sz < limVec::size() ) sz = limVec::size();

	if (  ((first_element_num == 0)  &&  !cut_head)  || (sz ==0 ) || (stlVec::size() == 0) || (limVec::size() == 0) )
	{
		if (last_element_num >= sz)
		{
			first_element_num = ( (sz) ?  0 : -1 ); 
			last_element_num = sz - 1;
		}
		stlVec::resize(sz, c);
	}	
	else	
	{	
		
		p_tempvec = new stlVec(sz);
		stlVec& tempvec = *(p_tempvec); 
		
		const long int min_size = ( (tempvec.size() > size()) ? size() : tempvec.size()  );
		if ( !cut_head )
		{			
			for (long int i=0; i<min_size; i++)
			{
				tempvec[i] = operator[](i);
			}
		}
		else
		{
			long int j=0;
			for (long int i= size() - min_size; i < size(); i++, j++ )
			{
				tempvec[j] = operator[](i);
			}
			
		}	
		
		stlVec::resize(sz, c);	
		first_element_num = -1;
		last_element_num = -1;
		for (long int i=0; i< min_size; i++)
		{
			limVec::push_back( tempvec[i]  );
		}
		
		
		delete p_tempvec;
		p_tempvec = 0;
	}
	
	check_elem_nums();
return stlVec:: size();	
}

template< typename Type, typename Allocator  >
void ArtLimVector<Type, Allocator>::clear(bool fast_clear)
{
	first_element_num = -1;
	last_element_num=  -1;
	
	if (!fast_clear)
	{
		long int max_size = stlVec::size();
		stlVec::clear();
		stlVec::resize(max_size);
	}
}



template< typename Type, typename Allocator  >
inline long int ArtLimVector<Type, Allocator>:: getMaxSize ()
{	
return stlVec::size();	
}

template< typename Type, typename Allocator  >
long int ArtLimVector<Type, Allocator>::resize ( long int sz, bool cut_head, bool increase_max_size, Type c)
{
	if (sz < 0) sz=0;
	if ( sz > stlVec::size()  )
	{
		if (!increase_max_size)  sz = stlVec::size();
		setMaxSize(sz, false, cut_head, c);
	}

	if ( sz >= size() )
	{
		const long int increase_size = sz - size ();
		if (cut_head)
		{
			for (int i=0; i < increase_size; i++)  push_front(c);
		}
		else
		{
			for (int i=0; i < increase_size; i++)  push_back(c);
		}
		
	}
	else
	{
		const long int decrease_size = size () - sz;
		if (cut_head)
		{
			for (int i=0; i < decrease_size; i++)  pop_front();
		}
		else
		{
			for (int i=0; i < decrease_size; i++)  pop_back();
		}
	}

return size();
}


template< typename Type, typename Allocator  >
inline void ArtLimVector<Type, Allocator>::resize(size_t sz, Type c)
{
	resize (sz, false, true, c);
}

template< typename Type, typename Allocator  >
inline void ArtLimVector<Type, Allocator>::check_elem_nums ()
{
		if (stlVec::size() == 0)
		{
				first_element_num = -1;
				last_element_num = -1;
		}
		else
		{
			if (first_element_num < -1) first_element_num = -1;
			if (last_element_num  < -1) last_element_num  = -1;
		}
}


/*
 * 
 * 		if ( !cut_head )
		{
			long int i_temp = 0;
			for (long int i=(last_element_num+1); (i < limVec::size()) && (i>=0) && (i_temp < tempvec.size()) ;  i++, i_temp++ )
			{
				tempvec[i_temp] = limVec::at(i);
			}
			
			for (long int i=0; (i<=last_element_num) && (i>=0) && (i_temp < tempvec.size()); i++, i_temp++)
			{
				tempvec[i_temp] = limVec::at(i);	
			}	
		}
		else
		{
			long int i_temp = p_tempvec->size() - 1;
			for (long int i=last_element_num; (i >=0)  &&  (i_temp >=0); i--, i_temp-- )
			{
				tempvec[i_temp] = limVec::at(i);
			}
			
			for (long int i=(limVec::size() - 1); (i > last_element_num) && (i>=0) && (i_temp >=0); i--, i_temp--)
			{
				tempvec[i_temp] = limVec::at(i);
			}
		}
*/
#endif