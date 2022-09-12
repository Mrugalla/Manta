#include "Label.h"

namespace gui
{
	struct Tooltip :
		public Comp
	{
		Tooltip(Utils&, String&&/*tooltip*/);
		
		void updateTooltip(const String*);

	protected:
		Label buildDateLabel, tooltipLabel;
		
		void paint(Graphics&) override;

		void resized() override;

	private:
		Notify makeNotify(Tooltip*);

		const String BuildDate;
	};
}