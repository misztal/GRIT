import pygrit as pg
import math

print('*** VORTEX IN A BOX ***\n')

cfg_filename = '../bin/tutorial_vortex_in_a_box.cfg'

settings  = pg.ConfigFile()
settings.load(cfg_filename)

parameters = pg.make_parameters_from_config_file( settings )

txt_filename = settings.get_value('txt_filename','circle_enright.txt')
output_path  = settings.get_value('output_path' ,''                  )
max_steps    = int(settings.get_value('steps','100'))

print('Loading mesh file: '+txt_filename)

engine = pg.Engine2D()

pg.init_engine_with_mesh_file(
                              pg.get_data_file_path(txt_filename)
                              , parameters
                              , engine
                              )

def write_svg_files( frame_number ):
    filename = output_path+pg.generate_filename('/tutorial_vortex_in_a_box',frame_number,'svg')
    pg.svg_draw(filename, engine, parameters)
    print('\t\twrite_svg_files() Done writting  '+filename)

def do_simulation_step( ):
    object_label = int(settings.get_value('object_label','1'))
    phase        = pg.make_phase(engine, object_label)
    px           = pg.VectorDouble()
    py           = pg.VectorDouble()

    pg.get_sub_range_current(engine, phase, px, py)

    N  = px.size()
    dt = 0.01
    
    px_new       = pg.VectorDouble(px)
    py_new       = pg.VectorDouble(py)
    
    for i in range(0,N):
        x = px[i]
        y = py[i]
        u =  2.0 * math.cos( math.pi * y ) * math.sin( math.pi * y ) * math.sin( math.pi*x ) * math.sin( math.pi*x )
        v = -2.0 * math.cos( math.pi * x ) * math.sin( math.pi * x ) * math.sin( math.pi*y ) * math.sin( math.pi*y )
        px_new[i] = px[i] + u*dt;
        py_new[i] = py[i] + v*dt;

    pg.set_sub_range_target(engine, phase, px_new, py_new)

write_svg_files(0)

for i in range(1, max_steps):
    do_simulation_step()
    engine.update(parameters)
    write_svg_files(i)

