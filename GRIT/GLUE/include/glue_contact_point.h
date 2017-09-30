#ifndef GLUE_CONTACT_POINT_H
#define GLUE_CONTACT_POINT_H

namespace glue
{
  /**
   * A Contact Point Class.
   * This is the basic contact point information we wish to compute. By
   * convention and to ensure uniqueness of contact points we define phase
   * A to be the phase with the lowest phase label value and the contact normal
   * is a unit vector always pointing from phase A to phase B.
   */
  class ContactPoint
  {
  public:

    double m_px;           ///< The x-coordinate of the contact point
    double m_py;           ///< The y-coordinate of the contact point
    double m_nx;           ///< The x-coordinate of the contact unit normal
    double m_ny;           ///< The y-coordinate of the contact unit normal

    int    m_A;            ///< The label of phase A
    int    m_B;            ///< The label of phase B
    int    m_vertex_A;     ///< The local vertex index in phase A
    int    m_vertex_B;     ///< The local vertex index in phase B

  public:

    /**
     * Requires normals to be exactly the same for two contact points to be the same. This
     * allows for having redundant positions but with different normals.
     */
    bool strong_compare(ContactPoint const & cp) const
    {
      if(this != &cp )
      {
        if( m_nx != cp.m_nx )
          return false;
        if( m_ny != cp.m_ny )
          return false;
        if( m_A != cp.m_A )
          return false;
        if( m_B != cp.m_B )
          return false;
        if( m_vertex_A != cp.m_vertex_A )
          return false;
        if( m_vertex_B != cp.m_vertex_B )
          return false;
      }
      return true;
    }

    /**
     * This comparison allows for different normals. That means that a contact
     * point is unique by its "position" and phase label pairs.
     */
    bool weak_compare(ContactPoint const & cp) const
    {
      if(this != &cp )
      {
        if( m_A != cp.m_A )
          return false;
        if( m_B != cp.m_B )
          return false;
        if( m_vertex_A != cp.m_vertex_A )
          return false;
        if( m_vertex_B != cp.m_vertex_B )
          return false;
      }
      return true;
    }

  public:

    bool operator==(ContactPoint const & cp) const
    {
      return this->strong_compare(cp);
    }

    bool operator!=(ContactPoint const & cp) const
    {
      return !(*this==cp);
    }

    bool operator<(ContactPoint const & cp) const
    {
      if (this->m_A > cp.m_A)
        return false;

      if (this->m_A < cp.m_A)
        return true;

      if (this->m_B > cp.m_B)
        return false;

      if (this->m_B < cp.m_B)
        return true;

      if (this->m_vertex_A > cp.m_vertex_A)
        return false;

      if (this->m_vertex_A < cp.m_vertex_A)
        return true;

      if (this->m_vertex_B > cp.m_vertex_B)
        return false;

      if (this->m_vertex_B < cp.m_vertex_B)
        return true;

      return true;
    }

  };

  inline ContactPoint make_contact_point(
                                         double const & px
                                         , double const & py
                                         , double const & nx
                                         , double const & ny
                                         , int const & A
                                         , int const & B
                                         , int const & vertex_A
                                         , int const & vertex_B
                                         )
  {
    ContactPoint cp;
    
    cp.m_px = px;
    cp.m_py = py;
    cp.m_nx = nx;
    cp.m_ny = ny;
    cp.m_A  = A;
    cp.m_B  = B;
    cp.m_vertex_A = vertex_A;
    cp.m_vertex_B = vertex_B;
    
    return cp;
  }
  
} // end namespace glue

// GLUE_CONTACT_POINT_H
#endif
