import numpy as np
import pygrit as GRIT
import math


def write_svg_files(engine, parameters, output_path, frame_number):
    filename = output_path + GRIT.generate_filename('/tutorial_vortex_in_a_box', frame_number, 'svg')
    GRIT.svg_draw(filename, engine, parameters)
    print('\t\twrite_svg_files() Done writing ' + filename)


def do_simulation_step(engine, settings):
    object_label = int(settings.get_value('object_label', '1'))
    phase = GRIT.make_phase(engine, object_label)
	
    N = np.asanyarray(phase.get_vertices()).size

    px = np.resize(np.array([]),N)
    py = np.resize(np.array([]),N)
	
    GRIT.get_sub_range_current(engine, phase, int(N), px, py)

    dt = 0.01
    
    px_new = np.array(px)
    py_new = np.array(py)
    
    for i in range(0, N):
        x = px[i]
        y = py[i]
        u = 2.0 * math.cos(math.pi * y) * math.sin(math.pi * y) * math.sin(math.pi*x) * math.sin(math.pi*x)
        v = -2.0 * math.cos(math.pi * x) * math.sin(math.pi * x) * math.sin(math.pi*y) * math.sin(math.pi*y)
        px_new[i] = px[i] + u*dt
        py_new[i] = py[i] + v*dt

    GRIT.set_sub_range_target(engine, phase, int(N), px_new, py_new)


def main():
    print('*** VORTEX IN A BOX ***\n')

    cfg_filename = '../bin/tutorial_vortex_in_a_box.cfg'

    settings = GRIT.ConfigFile()
    settings.load(cfg_filename)

    parameters = GRIT.make_parameters_from_config_file(settings)

    txt_filename = settings.get_value('txt_filename', 'circle_enright.txt')
    output_path = settings.get_value('output_path', '')
    max_steps = int(settings.get_value('steps', '100'))

    print('Loading mesh file: ' + txt_filename)

    engine = GRIT.Engine2D()

    GRIT.init_engine_with_mesh_file(
                              GRIT.get_data_file_path(txt_filename)
                              , parameters
                              , engine
                              )

    write_svg_files(engine, parameters, output_path, 0)

    for i in range(1, max_steps):
        do_simulation_step(engine, settings)
        engine.update(parameters)
        write_svg_files(engine, parameters, output_path, i)


if __name__ == "__main__":
    main()
