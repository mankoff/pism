def __getitem__(self, *args):
    return self.getitem(args[0][0], args[0][1])

def __setitem__(self, *args):
    if(len(args) == 2):
        self.setitem(args[0][0], args[0][1], args[1])
    else:
        raise ValueError("__setitem__ requires 2 arguments; received %d" % len(args))

def to_xarray(self, **kwargs):
    "Return an xarray.DataArray (a copy) containing data from this field (on rank 0)."
    tmp = self.allocate_proc0_copy()
    self.put_on_proc0(tmp)
    import numpy
    import xarray
    import cftime

    if self.grid().ctx().rank() == 0:
        data = numpy.array(tmp.get()).reshape(1, *self.shape())
        grid = self.grid()
        time = grid.ctx().time()
        calendar = time.calendar()
        units = time.units_string()
        date = cftime.num2date(time.current(), units, calendar)
        spatial_coords = self.spatial_coords
        metadata = self.metadata()
        name = metadata.get_string("short_name")
        if metadata.n_spatial_dimensions() > 2:
            dims = ["time", "y", "x", "z"]
        else:
            dims = ["time", "y", "x"]
            
        attrs = self.attrs
        # Spatial coordinates
        coords = {k: ([k], numpy.array(grid[k]), spatial_coords[k]) for k in spatial_coords.keys()}
        # Temporal coordinate
        coords["time"] =  (["time"], xarray.cftime_range(date, periods=1), {})
        return xarray.DataArray(data, coords=coords, dims=dims, attrs=attrs, name=name, **kwargs)
    else:
        return None

def imshow(self, **kwargs):
    "Plot a 2D field using matplotlib.pylab.imshow()."

    try:
        import matplotlib.pylab as plt
    except:
        raise RuntimeError("Failed to import matplotlib.pylab. Please make sure that matplotlib is installed!")

    m = plt.imshow(self.to_numpy(), origin="lower", **kwargs)
    plt.colorbar(m)
    md = self.metadata()
    plt.title("{}, {}".format(md.get_string("long_name"), md.get_string("units")))
    plt.axis("equal")
