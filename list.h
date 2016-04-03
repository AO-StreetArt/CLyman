#ifndef LIST_H
#define LIST_H

template <class T>
class List
{
  T *int_array;
  int cur_int_array_size;
  int current_length;
  void extend_array()
{
  int cur_len = 0;
  int n=0;
  cur_len = cur_int_array_size;
  T *new_array = new T[cur_len+10];
  for (n=0;n<cur_int_array_size;n++)
  {
    new_array[n] = int_array[n];
  }
  delete[] int_array;
  int_array = new_array;
  cur_int_array_size=cur_int_array_size+10;
}
  public:
    List() {current_length=0;cur_int_array_size=10;int_array=new T[10];}
    ~List() {delete[] int_array;}
    T get (int index) {return int_array[index];}
    void append (T obj) {if (current_length == cur_int_array_size) {extend_array();} int_array[current_length+1] = obj; current_length++;}
    void insert (int index, T obj) {if (current_length == cur_int_array_size) {extend_array();} int_array[index] = obj; current_length++;}
    void remove (int index) {int i=index;for (i=index; i < cur_int_array_size - 1; i++) {int_array[i] = int_array[i+1];}current_length--;}
    int length () {return current_length;}
};
#endif
