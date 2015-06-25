if [ $USER = "root" ]; then
  HASREADLINK=`which readlink`
  JAVAC=`which javac`
  getjdkinclude()
  {
    if [ -n "$HASREADLINK" ]; then 
      while [ -h $JAVAC ]
      do
        JAVAC=`readlink $JAVAC`
      done
      echo `dirname $(dirname $JAVAC)`/$JDKINCLUDEDIRNAME
    else
      echo "need readlink. please install."
    fi
    return 
  }
  if [ `uname` = "Darwin" ]; then
    LIBSUFFIX="dylib"
    JDKINCLUDEDIRNAME="Headers"
  else
    LIBSUFFIX="so"
    JDKINCLUDEDIRNAME="include"
  fi
  JDKINC=`getjdkinclude`
  JDKINCMD="$JDKINC/linux"
  gcc -c -DHAVE_EXPAT_CONFIG_H -DHAVE_MEMMOVE -DJNI -I$JDKINC -I$JDKINCMD -Iinclude -O0 -Wall -g3 -fPIC \
    src/*.c  
  gcc *.o -shared -lexpat -laspell -ltidy -licuuc -licuio -o libAeseStripper.$LIBSUFFIX
  cp libAeseStripper.$LIBSUFFIX /usr/local/lib
else
	echo "Need to be root. Did you use sudo?"
fi

