/* Class Rectangle */
class Rectangle : public Shape
{
public:
  Rectangle (int x, int y, int w, int h);
  virtual void Draw ();
  virtual void MoveTo (int newx, int newy);
  virtual void RMoveTo (int dx, int dy);
  virtual void SetWidth (int newWidth);
  virtual void SetHeight (int newHeight);

private:
  int x, y;
  int width;
  int height;
};
