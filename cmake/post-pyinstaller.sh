patchelf --set-rpath    '$ORIGIN'       array.so                   
patchelf --set-rpath    '$ORIGIN'       binascii.so                
patchelf --set-rpath    '$ORIGIN'       _bisect.so                 
patchelf --set-rpath    '$ORIGIN'       bz2.so                     
patchelf --set-rpath    '$ORIGIN'       cmath.so                   
patchelf --set-rpath    '$ORIGIN'       _codecs_cn.so              
patchelf --set-rpath    '$ORIGIN'       _codecs_hk.so              
patchelf --set-rpath    '$ORIGIN'       _codecs_iso2022.so         
patchelf --set-rpath    '$ORIGIN'       _codecs_jp.so              
patchelf --set-rpath    '$ORIGIN'       _codecs_kr.so              
patchelf --set-rpath    '$ORIGIN'       _codecs_tw.so              
patchelf --set-rpath    '$ORIGIN'       _collections.so            
patchelf --set-rpath    '$ORIGIN'       cPickle.so                 
patchelf --set-rpath    '$ORIGIN'       cStringIO.so               
patchelf --set-rpath    '$ORIGIN'       _ctypes.so                 
patchelf --set-rpath    '$ORIGIN'       datetime.so                
patchelf --set-rpath    '$ORIGIN'       fcntl.so                   
patchelf --set-rpath    '$ORIGIN'       _functools.so              
patchelf --set-rpath    '$ORIGIN'       _hashlib.so                
patchelf --set-rpath    '$ORIGIN'       _heapq.so                  
patchelf --set-rpath    '$ORIGIN'       _io.so                     
patchelf --set-rpath    '$ORIGIN'       itertools.so               
patchelf --set-rpath    '$ORIGIN'       libpython2.7.so.1.0        
patchelf --set-rpath    '$ORIGIN'       _locale.so                 
patchelf --set-rpath    '$ORIGIN'       math.so                    
patchelf --set-rpath    '$ORIGIN'       _multibytecodec.so         
patchelf --set-rpath    '$ORIGIN'       operator.so                
patchelf --set-rpath    '$ORIGIN'       pyexpat.so                 
patchelf --set-rpath    '$ORIGIN'       _random.so                 
patchelf --set-rpath    '$ORIGIN'       readline.so                
patchelf --set-rpath    '$ORIGIN'       resource.so                
patchelf --set-rpath    '$ORIGIN'       select.so                  
patchelf --set-rpath    '$ORIGIN'       _socket.so                 
patchelf --set-rpath    '$ORIGIN'       _ssl.so                    
patchelf --set-rpath    '$ORIGIN'       strop.so                   
patchelf --set-rpath    '$ORIGIN'       _struct.so                 
patchelf --set-rpath    '$ORIGIN'       termios.so                 
patchelf --set-rpath    '$ORIGIN'       time.so                    
patchelf --set-rpath    '$ORIGIN'       unicodedata.so             
patchelf --set-rpath    '$ORIGIN'       zlib.so                    

rm libX11.so.6
rm libXext.so.6 libXfixes.so.3 
rm libXinerama.so.1 libXr*
rm libXcursor.so.1 
rm libXi.so.6 
rm libXxf86vm.so.1 
