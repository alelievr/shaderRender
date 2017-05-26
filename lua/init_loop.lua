--package.path = RESOURCE_DIR .. "/?.lua;"  .. package.path
--print(package.path)

function init_oo()
    print( "package.path was    : "..package.path )
    package.path = "./lua/loop30/?.lua;"..package.path
    print( "package.path now is : "..package.path )
    oo = require "loop/simple"
    if oo then
        print( "LOOP loaded from C++" )
        --oo = loop.simple
    else
        print( "LOOP load from C++ failed" )
    end
end
