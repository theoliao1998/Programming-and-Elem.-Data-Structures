#ifndef P5_DLIST_IMPL_H
#define P5_DLIST_IMPL_H

template <class T>
bool Dlist<T>::isEmpty() const {
    return !first && !last;
}

template <class T>
void Dlist<T>::insertFront(T *op) {
    node * np=new node;
    np->next=first;
    np->prev=NULL;
    np->op=op;
    if(isEmpty())last=np;
    else first->prev=np;
    first=np;
}

template <class T>
void Dlist<T>::insertBack(T *op) {
    node * np=new node;
    np->next=NULL;
    np->prev=last;
    np->op=op;
    if(isEmpty())first=np;
    else last->next=np;
    last=np;
}

template <class T>
T* Dlist<T>::removeFront() {
    if(isEmpty()){
        emptyList e;
        throw e;
    }
    node *victim=first;
    first=victim->next;
    if(first==NULL){
        last=NULL;
    }
    else {
        first->prev=NULL;
    }
    T* result=victim->op;
    delete victim;
    return result;
}

template <class T>
T* Dlist<T>::removeBack() {
    if(isEmpty()){
        emptyList e;
        throw e;
    }
    node *victim=last;
    last=victim->prev;
    if(last==NULL){
        first=NULL;
    }
    else {
        last->next=NULL;
    }
    T* result=victim->op;
    delete victim;
    return result;
}

template <class T>
Dlist<T>::Dlist()
: first(0),last(0) {
}

template <class T>
Dlist<T>::Dlist(const Dlist &l)
: first(0),last(0) {
    copyAll(l);
}

template <class T>
Dlist<T>& Dlist<T>::operator=(const Dlist &l) {
    if(this!=&l) {
        removeAll();
        copyAll(l);
    }
    return *this;
}

template <class T>
Dlist<T>::~Dlist() {
    removeAll();
}

template <class T>
void Dlist<T>::removeAll() {
    while(!isEmpty()){
        delete removeFront();
    }
}

template <class T>
void Dlist<T>::copyAll(const Dlist &l) {
    first=last=NULL;
    node* start=l.first;
    if(!l.isEmpty()){
        while(start!=NULL){
            insertBack(new T(*start->op));
            start=start->next;
        }
    }
}

#endif //P5_DLIST_IMPL_H
