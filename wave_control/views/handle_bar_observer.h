#pragma once

// HandleBarObserver is notified about the HandleBar change.
// OnHandleBarPressed and OnHandleReleased are match
// OnHandleMove may call out of Pressed and Released, for direct set pos.
class HandleBarObserver {
public:
  virtual void OnHandlePressed(int id, int offset, bool horiz) = 0;

  // Invoked when the Handle is be draged and move.
  virtual void OnHandleMove(int ID, int offset) = 0;

  virtual void OnHandleReleased(int id) = 0;

  // Invoked when the Handle be clicked.
  virtual void OnHandleActive(int ID) = 0;

protected:
  virtual ~HandleBarObserver() {}
};
