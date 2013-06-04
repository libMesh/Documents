/* Class Circle */
class Circle : public Shape
{
public:
  Circle (int initx, int inity, int initr);
  virtual void Draw ();
  virtual void MoveTo (int newx, int newy);
  virtual void RMoveTo (int dx, int dy);
  virtual void SetRadius (int newRadius);
  
private:
  int x, y;
  int radius;
};
