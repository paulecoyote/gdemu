
namespace Lander 
{
  class LanderGame
  {
  public:
    LanderGame(){};
    ~LanderGame(){};

    bool Play(void);

  private:
    // Prevent automatic generation of 
    LanderGame( const LanderGame& );
    LanderGame& operator=( const LanderGame& );
  };
}