/* abstract interface declaration */
class Shape
{
public:
  virtual void Draw () = 0;
  virtual void MoveTo (int newx, int newy) = 0;
  virtual void RMoveTo (int dx, int dy) = 0;
};
