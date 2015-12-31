### Building
To build the executable use the rebuild.sh script
To rebuild the .so use the rebuildll.sh script

Adjust it if you want it put somewhere else. When you run Java it does 
not look in the /usr/local/lib directory for libraries unless you set 
the java.library.path when you launch Java, such as: 
-Djava.library.path=.:/usr/java/packages/lib/amd64:/usr/lib64:/lib64:/lib:/usr/lib:/usr/local/lib 

To find the Java libpath use a trivial Java program like
public class LibPath
{
    public static void main( String[] args )
    {
        System.out.print(System.getProperty("java.library.path"));
    }
}
Alternatively, just edit the script to put it in one of the standard 
directories like /usr/lib.

Dependencies are:
libtidy-dev
libicu52
libicu-dev
icu-devtools
libaspell-dev
libexpat1
libexpat1-dev


### Using the stripper from teh Java program
The stripper is intended to be called from a Java program. The 
AeseStripper class in calliope.core should be used. There is an 
example of its use in StageThreeXML.java in the Importer tool:

AeseStripper stripper = new AeseStripper();
int res = stripper.strip( xml, stripConfig, 
    style, dict, hhExcepts, 
    is_xml, text, markup );

where xml is your xml file as a String

stripConfig can be null or a JSON config file which is used to 
combine element names and attributes. There are several examples 
in the distribution, but it is probably best left as null.

style is a name of a database key that will be added to the STIL 
file. This can be anything. I use "TEI/default" mostly.

dict is the name of an aspell dict used to dehyphenate the source. 
The default value is "en_GB", but can be the name of any installed 
aspell dictionary.

hhExcepts a space-separated list of merged hard-hyphen words like 
"thunder-head dog-house" can be NULL. You should only specify 
hyphenated words that consist of two words in the dictionary 
joined by hyphen that you do NOT want merged at line-end.

is_xml set to true unless you are stripping html

text a JSONResponse object to hold the stripped text. This is in 
the calliope.core jar

markup a JSONResponse object to hold the STIL markup result.

