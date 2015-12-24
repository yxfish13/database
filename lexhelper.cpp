#define MAXCHAR		5000
#define MAXSTRINGLEN		255

static char charpool[MAXCHAR];
static int charptr = 0;

char *string_alloc(int len)
{
   	char *s;

   	if(charptr + len > MAXCHAR){
      		exit(1);
   	}

   	s = charpool + charptr;
   	charptr += len;

   	return s;
}

void reset_charptr(void)
{
    charptr = 0;
}

int lower(char *dst, char *src, int max);
char *mk_string(char *s, int len);

int get_id(char *s)
{
   char string[MAXSTRINGLEN];
   int len;

   if((len = lower(string, s, MAXSTRINGLEN)) == MAXSTRINGLEN)
      return NOTOKEN;

   /*  SM layer lexemes */

   if(!strcmp(string,"create"))
      return yylval.ival = T_CREATE;
   if(!strcmp(string,"drop"))
      return yylval.ival = T_DROP;
   if(!strcmp(string,"use"))
      return yylval.ival = T_USE;
   if(!strcmp(string,"database"))
      return yylval.ival = T_DATABASE;
   if(!strcmp(string,"show"))
      return yylval.ival = T_SHOW;
   if(!strcmp(string,"desc"))
      return yylval.ival = T_DESC;
   if(!strcmp(string,"tables"))
      return yylval.ival = T_TABLES;
   if(!strcmp(string,"table"))
      return yylval.ival = T_TABLE;
   if(!strcmp(string,"not"))
      return yylval.ival = T_NOT;
   if(!strcmp(string,"null"))
      return yylval.ival = T_NULL;
   if(!strcmp(string,"set"))
      return yylval.ival = T_SET;

   if(!strcmp(string,"and"))
      return yylval.ival = T_AND;

   if(!strcmp(string,"into"))
      return yylval.ival = T_INTO;
   if(!strcmp(string,"values"))
      return yylval.ival = T_VALUES;

   if(!strcmp(string,"select"))
      return yylval.ival = T_SELECT;
   if(!strcmp(string,"from"))
      return yylval.ival = T_FROM;
   if(!strcmp(string,"where"))
      return yylval.ival = T_WHERE;
   if(!strcmp(string,"insert"))
      return yylval.ival = T_INSERT;
   if(!strcmp(string,"delete"))
      return yylval.ival = T_DELETE;
   if(!strcmp(string,"update"))
      return yylval.ival = T_UPDATE;

   /*  unresolved lexemes are strings */

   yylval.sval = mk_string(s, len);
   return T_STRING;
}

/*
 * lower: copies src to dst, converting it to lowercase, stopping at the
 * end of src or after max characters.
 *
 * Returns:
 * 	the length of dst (which may be less than the length of src, if
 * 	    src is too long).
 */
int lower(char *dst, char *src, int max)
{
   int len;

   for(len = 0; len < max && src[len] != '\0'; ++len){
      dst[len] = src[len];
      if(src[len] >= 'A' && src[len] <= 'Z')
         dst[len] += 'a' - 'A';
   }
   dst[len] = '\0';

   return len;
}

char *get_qstring(char *qstring, int len)
{
   	qstring[len - 1] = '\0';

   	return mk_string(qstring + 1, len - 2);
}

char *mk_string(char *s, int len)
{
   	char *copy;

   	if((copy = string_alloc(len + 1)) == NULL){
      		printf("out of string space\n");
      		exit(1);
   	}

   	strncpy(copy, s, len + 1);
   	return copy;
}