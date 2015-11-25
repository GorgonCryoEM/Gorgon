echo -e '\033[31;1m'"FIXING RPATH"'\033[0m'

patchelf --debug --set-rpath    '$ORIGIN'       array.so                   
patchelf --debug --set-rpath    '$ORIGIN'       binascii.so                
patchelf --debug --set-rpath    '$ORIGIN'       _bisect.so                 
patchelf --debug --set-rpath    '$ORIGIN'       bz2.so                     
patchelf --debug --set-rpath    '$ORIGIN'       cmath.so                   
patchelf --debug --set-rpath    '$ORIGIN'       _codecs_cn.so              
patchelf --debug --set-rpath    '$ORIGIN'       _codecs_hk.so              
patchelf --debug --set-rpath    '$ORIGIN'       _codecs_iso2022.so         
patchelf --debug --set-rpath    '$ORIGIN'       _codecs_jp.so              
patchelf --debug --set-rpath    '$ORIGIN'       _codecs_kr.so              
patchelf --debug --set-rpath    '$ORIGIN'       _codecs_tw.so              
patchelf --debug --set-rpath    '$ORIGIN'       _collections.so            
patchelf --debug --set-rpath    '$ORIGIN'       cPickle.so                 
patchelf --debug --set-rpath    '$ORIGIN'       cStringIO.so               
patchelf --debug --set-rpath    '$ORIGIN'       _ctypes.so                 
patchelf --debug --set-rpath    '$ORIGIN'       datetime.so                
patchelf --debug --set-rpath    '$ORIGIN'       fcntl.so                   
patchelf --debug --set-rpath    '$ORIGIN'       _functools.so              
patchelf --debug --set-rpath    '$ORIGIN'       _hashlib.so                
patchelf --debug --set-rpath    '$ORIGIN'       _heapq.so                  
patchelf --debug --set-rpath    '$ORIGIN'       _io.so                     
patchelf --debug --set-rpath    '$ORIGIN'       itertools.so               
patchelf --debug --set-rpath    '$ORIGIN'       libpython2.7.so.1.0        
patchelf --debug --set-rpath    '$ORIGIN'       _locale.so                 
patchelf --debug --set-rpath    '$ORIGIN'       math.so                    
patchelf --debug --set-rpath    '$ORIGIN'       _multibytecodec.so         
patchelf --debug --set-rpath    '$ORIGIN'       operator.so                
patchelf --debug --set-rpath    '$ORIGIN'       pyexpat.so                 
patchelf --debug --set-rpath    '$ORIGIN'       _random.so                 
patchelf --debug --set-rpath    '$ORIGIN'       readline.so                
patchelf --debug --set-rpath    '$ORIGIN'       resource.so                
patchelf --debug --set-rpath    '$ORIGIN'       select.so                  
patchelf --debug --set-rpath    '$ORIGIN'       _socket.so                 
patchelf --debug --set-rpath    '$ORIGIN'       _ssl.so                    
patchelf --debug --set-rpath    '$ORIGIN'       strop.so                   
patchelf --debug --set-rpath    '$ORIGIN'       _struct.so                 
patchelf --debug --set-rpath    '$ORIGIN'       termios.so                 
patchelf --debug --set-rpath    '$ORIGIN'       time.so                    
patchelf --debug --set-rpath    '$ORIGIN'       unicodedata.so             
patchelf --debug --set-rpath    '$ORIGIN'       zlib.so                    

echo -e '\033[32;1m'"REMOVING FILES"'\033[0m'

rm -v libX11.so.6
rm -v libXext.so.6 libXfixes.so.3 
rm -v libXinerama.so.1 libXr*
rm -v libXcursor.so.1 
rm -v libXi.so.6 
rm -v libXxf86vm.so.1 
