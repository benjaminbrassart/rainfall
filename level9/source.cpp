struct N;

typedef int (weird_func_t)(N &, N &);

struct N {
  weird_func_t *func;
  char annotation[100];
  int value;

  N(int value)
  {
    this->func = (weird_func_t *)&N::operator+;
    this->value = value;
  }

  int operator-(N &rhs) {
    return this->value - rhs.value;
  }

  int operator+(N &rhs) {
    return this->value + rhs.value;
  }

  void setAnnotation(char *s)
  {
    size_t len = strlen(s);

    memcpy(this->annotation, s, len);
  }
};

int main(int argc, char **argv)
{
  if (argc < 2) {
    _exit(1);
  }

  N *n1 = new N(5);
  N *n2 = new N(6);

  n1->setAnnotation(argv[1]);
  n2->func(*n2, *n1);
}
