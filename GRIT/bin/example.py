import pygrit as GRIT
import numpy as np
import math


def main():
    cfg_filename = '../bin/example.cfg'

    settings = GRIT.ConfigFile()
    settings.load(cfg_filename)

    parameters = GRIT.make_parameters_from_config_file(settings)

    txt_filename = settings.get_value('txt_filename', 'circle_enright.txt')

    engine = GRIT.Engine2D()

    GRIT.init_engine_with_mesh_file(
                              GRIT.get_data_file_path(txt_filename)
                              , parameters
                              , engine
                              )

    object_label = int(settings.get_value('object_label', '1'))
    phase = GRIT.make_phase(engine, object_label)    

    vertices = np.asanyarray(phase.get_vertices())
    triangles = np.asanyarray(phase.get_triangles())

    print(vertices)
    print(triangles)

    
if __name__ == "__main__":
    main()
