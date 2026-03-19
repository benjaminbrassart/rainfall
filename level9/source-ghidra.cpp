int main(int argc,char **argv)

{
  N *n1;
  N *n2;
  int iVar1;

  if (argc < 2) {
                    /* WARNING: Subroutine does not return */
    _exit(1);
  }
  n1 = operator.new(108);
  N::N(n1,5);
  n2 = operator.new(108);
  N::N(n2,6);
  N::setAnnotation(n1,argv[1]);
  iVar1 = (*(code *)**(undefined4 **)n2)(n2,n1);
  return iVar1;
}



/* N::N(int) */
void __thiscall N::N(N *this,int param_1)

{
  *(undefined ***)this = &PTR_operator+_08048848;
  *(int *)(this + 0x68) = param_1;
  return;
}



/* N::TEMPNAMEPLACEHOLDERVALUE(N&) */
int __thiscall N::operator+(N *this,N *param_1)

{
  return *(int *)(param_1 + 0x68) + *(int *)(this + 0x68);
}



/* N::setAnnotation(char*) */
void __thiscall N::setAnnotation(N *this,char *param_1)

{
  size_t __n;

  __n = strlen(param_1);
  memcpy(this + 4,param_1,__n);
  return;
}
