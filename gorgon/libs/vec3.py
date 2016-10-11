from ..toolkit.libpytoolkit import Vec3F


class Vec3(Vec3F):
        
    def __init__(self, *args, **kwargs):
        if kwargs:
            raise Exception("Wrong named-arguments")
        if len(args) == 1 and (isinstance(args[0], list) or isinstance(args[0], Vec3F) or isinstance(args[0], tuple)):
            [x, y, z] = [args[0][i] for i in range(3)]
        elif len(args) == 3:
            [x, y, z] = [args[i] for i in range(3)]
        else:
            raise Exception("Wrong arguments:\nargs:%s\n%s" %(args, kwargs))

        super(Vec3, self).__init__(x, y, z)

    def __repr__(self):
        return "<Vec3(%f, %f, %f)>" % (self[0], self[1], self[2])
