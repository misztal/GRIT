import pygrit as GRIT
import numpy as np
import math

# Decide upon some names for creating user-custom attribute fields,
# they can be anything you want as long as they are unique.
refinement_attribute_name = 'my_refinement_values'
coarsening_attribute_name = 'my_coarsening_values'

def write_svg_files(engine, parameters, output_path, frame_number):
    filename = output_path + GRIT.generate_filename('/tutorial_contact', frame_number, 'svg')
    GRIT.svg_draw(filename, engine, parameters)
    print('\t\twrite_svg_files() Done writing ' + filename)
	
def create_sizing_fields(engine, settings, parameters):
    # Now create the actual edge attribute fields that will control maximum
    # edge length before doing a refinement and the lowest edge length
    # before doing coarsening.
    engine.create_attribute(refinement_attribute_name, 1)
    engine.create_attribute(coarsening_attribute_name, 1)
 
    # Fill in some sensible initial values for the refinement and coarsening
    # edge length values. Make sure coarsening is lower than refinement.
    refinement_value = float(settings.get_value('custom_refinement_value','0.5'))
    coarsening_value = float(settings.get_value('custom_coarsening_value','0.1'))

    GRIT.clear_attribute( 
                         engine
                         , refinement_attribute_name
                         , refinement_value
                         , GRIT.EDGE_ATTRIBUTE()
                         )

    GRIT.clear_attribute(
                         engine
                         , coarsening_attribute_name
                         , coarsening_value
                         , GRIT.EDGE_ATTRIBUTE()
                         )

    # Next connect the user defined edge attribute fields to the actual
    # mesh algorithm operations that do refinement and coarsening.
    parameters.set_lower_threshold_attribute('refinement',           refinement_attribute_name)
    parameters.set_lower_threshold_attribute('interface_refinement', refinement_attribute_name)
    parameters.set_upper_threshold_attribute('coarsening',           coarsening_attribute_name)
    parameters.set_upper_threshold_attribute('interface_coarsening', coarsening_attribute_name)
	
# Custom condition associated with Engine2D.filter function
# @param engine    Engine2D type object
# @param simplex   Object of type Simplex0, Simplex1, Simplex2, used with logical expression functions
# @param info      Custom object which can be used to arbitrary parameters to the function
# Any local function used as a condition for Engine2D.filter has to take these 3 arguments
def is_object(engine, simplex, info):
    return engine.in_phase(info.label1,simplex) or engine.in_phase(info.label2,simplex)

# Vertices (dim 0 simplices) are needed to create a closed data set (required by GRIT.make_phase)
def is_contact_surface(engine, simplex, info):
    if engine.is_dimension(1,simplex) or engine.is_dimension(0,simplex):
        return engine.in_phase(info.label1,simplex) and engine.in_phase(info.label2,simplex) 
        

def update_sizing_fields(engine, settings, parameters):
    north_object_label = int(settings.get_value('north_object_label','1'))
    south_object_label = int(settings.get_value('south_object_label','2'))
  
    refinement_value = float(settings.get_value('custom_refinement_value','0.5'))
    coarsening_value = float(settings.get_value('custom_coarsening_value','0.1'))

    GRIT.clear_attribute(
	                     engine
                         , refinement_attribute_name
                         , refinement_value
                         , GRIT.EDGE_ATTRIBUTE()
                         )

    GRIT.clear_attribute( 
                         engine
                         , coarsening_attribute_name
                         , coarsening_value
                         , GRIT.EDGE_ATTRIBUTE()
                         )

    all = engine.get_all_simplices()
	
	# Put here all parameters required by the condition function
    class Info:
        label1 = north_object_label
        label2 = south_object_label		
	
    objects = GRIT.make_phase(engine, engine.filter(all,__name__,'is_object',Info()))

    N = np.asanyarray(objects.get_edges()).size

    refinement_values = np.full(N, 0.025)
    coarsening_values = np.full(N, 0.01)
	
    GRIT.set_sub_range( engine, objects, refinement_attribute_name, refinement_values, GRIT.EDGE_ATTRIBUTE())
    GRIT.set_sub_range( engine, objects, coarsening_attribute_name, coarsening_values, GRIT.EDGE_ATTRIBUTE())
  
    # Extract the contact surface
    contact = engine.filter(all,__name__,'is_contact_surface',Info())
    contact_line = GRIT.make_phase(engine, contact) 
    
    M = np.asanyarray(contact_line.get_edges()).size
	
    refinement_values = np.full(M, 0.0125)
    coarsening_values = np.full(M, 0.005)

    GRIT.set_sub_range( engine, contact_line, refinement_attribute_name, refinement_values, GRIT.EDGE_ATTRIBUTE());
    GRIT.set_sub_range( engine, contact_line, coarsening_attribute_name, coarsening_values, GRIT.EDGE_ATTRIBUTE());

def do_simulation_step(engine, settings):
    move_x = float(settings.get_value('move_x','0.01'))
    move_y = float(settings.get_value('move_y','0.01'))
	
    north_object_label = int(settings.get_value('north_object_label','1'))
    south_object_label = int(settings.get_value('south_object_label','2'))
	
    north_phase = GRIT.make_phase(engine, north_object_label)
    south_phase = GRIT.make_phase(engine, south_object_label)
	
    NN = np.asanyarray(north_phase.get_vertices()).size
    NS = np.asanyarray(south_phase.get_vertices()).size
	
    north_px = np.resize(np.array([]),NN)
    north_py = np.resize(np.array([]),NN)
    south_px = np.resize(np.array([]),NS)
    south_py = np.resize(np.array([]),NS)

    GRIT.get_sub_range_current(engine, north_phase, north_px, north_py)
    GRIT.get_sub_range_current(engine, south_phase, south_px, south_py)
    
    for i in range(0, NN):
        north_px[i] += move_x
        north_py[i] += move_y
		
    for i in range(0, NS):
        south_px[i] -= move_x
        south_py[i] -= move_y

    GRIT.set_sub_range_target(engine, north_phase, north_px, north_py)
    GRIT.set_sub_range_target(engine, south_phase, south_px, south_py)
	
def main():
    print('*** CONFORMING CONTACT ***\n')

    cfg_filename = '../bin/tutorial_contact.cfg'

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

    use_sizing_fields = bool(settings.get_value('use_sizing_fields','true'))
	
    if use_sizing_fields:
        create_sizing_fields(engine, settings, parameters)
							  
    write_svg_files(engine, parameters, output_path, 0)

    for i in range(1, max_steps):
        do_simulation_step(engine, settings)
        if use_sizing_fields:
            update_sizing_fields(engine, settings, parameters)
            print('Updated sizing fields')
        engine.update(parameters)
        write_svg_files(engine, parameters, output_path, i)


if __name__ == "__main__":
    main()
