#include "XyPad.h"

namespace Gui
{
	/*
	 * XY Pad Thumb section
	 */
	XyPad::Thumb::Thumb()
	{
		constrainer.setMinimumOnscreenAmounts(thumbSize, thumbSize, thumbSize, thumbSize);
	}

	void XyPad::Thumb::paint(Graphics& g)
	{
		g.setColour(Colours::white);
		g.fillEllipse(getLocalBounds().toFloat());
	}

	void XyPad::Thumb::mouseDown(const MouseEvent& event)
	{
		dragger.startDraggingComponent(this, event);
	}

	void XyPad::Thumb::mouseDrag(const MouseEvent& event)
	{
		dragger.dragComponent(this, event, &constrainer);
		if (moveCallback)
			moveCallback(getPosition().toDouble());
	}

	/*
	 * XY Pad section
	 */
	XyPad::XyPad()
	{
		addAndMakeVisible(thumb);
		thumb.moveCallback = [&](Point<double> position)
		{
				const auto bounds = getLocalBounds().toDouble();
				const auto w = static_cast<double>(thumbSize);

				double normalizedX = (position.getX() / (bounds.getWidth() - w)) - 0.5;

				double mappedDelayValue = jmap(normalizedX, -0.5, 0.5, -17.5, 17.5);

				DBG("Position X: " << position.getX());
				DBG("Normalized X: " << normalizedX);
				DBG("Mapped Delay Value: " << mappedDelayValue);

				for (auto* slider : xSliders)
				{
					slider->setValue(mappedDelayValue, sendNotification);
				}

			for (auto* slider : ySliders)
			{
				slider->setValue(jmap(position.getY(), bounds.getHeight() - w, 0.0, slider->getMinimum(), slider->getMaximum()));
			}
		};
	}

	void XyPad::resized()
	{
		thumb.setBounds(getLocalBounds().withSizeKeepingCentre(thumbSize, thumbSize));
		if (!xSliders.empty())
			sliderValueChanged(xSliders[0]);
		if (!ySliders.empty())
			sliderValueChanged(ySliders[0]);
	}

	void XyPad::paint(Graphics& g)
	{
		g.setColour(Colours::black);
		g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.f);
	}

	void XyPad::registerSlider(Slider* slider, Axis axis)
	{
		slider->addListener(this);
		const std::lock_guard<std::mutex> lock(vectorMutex);		
		if (axis == Axis::X)
			xSliders.push_back(slider);
		if (axis == Axis::Y)
			ySliders.push_back(slider);
	}

	void XyPad::deregisterSlider(Slider* slider)
	{
		slider->removeListener(this);
		const std::lock_guard<std::mutex> lock(vectorMutex);
		// remove/erase idiom
		xSliders.erase(std::remove(xSliders.begin(), xSliders.end(), slider), xSliders.end());
		ySliders.erase(std::remove(ySliders.begin(), ySliders.end(), slider), ySliders.end());
	}

	void XyPad::sliderValueChanged(Slider* slider)
	{
		// Avoid loopback
		if (thumb.isMouseOverOrDragging(false))
			return;
		
		// Figure out if the slider belongs to xSliders or ySliders
		const auto isXAxisSlider = std::find(xSliders.begin(), xSliders.end(), slider) != xSliders.end();
		const auto bounds = getLocalBounds().toDouble();
		const auto w = static_cast<double>(thumbSize);
		if (isXAxisSlider)
		{
			thumb.setTopLeftPosition(
				jmap(slider->getValue(), slider->getMinimum(), slider->getMaximum(), 0.0, bounds.getWidth() - w),
				thumb.getY());
		} else
		{
			thumb.setTopLeftPosition(
				thumb.getX(),
				jmap(slider->getValue(), slider->getMinimum(), slider->getMaximum(), bounds.getHeight() - w, 0.0));
		}
		repaint();
	}
}
