

import pygahm

start_date = pygahm.Date(2005,8,25)
end_date = pygahm.Date(2005,8,26)
dt = 1800

                           #  xll  yll xur yur dx  dy
wind_domain = pygahm.WindGrid(-99, 15, -75, 35,0.1,0.1)

owi = pygahm.OwiAscii(start_date,end_date,dt)

owi.addDomain(wind_domain,"katrina.221","katrina.222")

xloc = wind_domain.xpoints();
yloc = wind_domain.ypoints();

gahm_model = pygahm.GahmVortex("../testing/test_files/bal122005.dat",xloc,yloc)

for d in range(start_date.toSeconds(),end_date.toSeconds(),dt):
    now = pygahm.Date(d)
    print("Time = ",now.toString())
    print("   Solving GAHM...",end='')
    value = gahm_model.get(now)
    print("done!")
    print("Writing solution to Oceanweather format...",end='')
    owi.write(now,0,value)
    print("done!")

