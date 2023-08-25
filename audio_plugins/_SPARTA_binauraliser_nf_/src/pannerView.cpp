/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.1.6

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "pannerView.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

//[/MiscUserDefs]

//==============================================================================
pannerView::pannerView (PluginProcessor* ownerFilter, int _width, int _height)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]


    //[UserPreSize]
    //[/UserPreSize]

    setSize (492, 246);


    //[Constructor] You can add your own custom stuff here..
    hVst = ownerFilter;
    hBin = hVst->getFXHandle();
    width = _width;
    height = _height;
    halfWidth = width / 2.0f;
    halfHeight = height / 2.0f;
    icon_diam_src = height / 25.0f;
    icon_radius_src = icon_diam_src / 2.0f;
    icon_diam_hrir = height / 45.0f;
    icon_radius_hrir = icon_diam_src / 2.0f;
    ffThresh = binauraliserNF_getFarfieldThresh_m(hBin);
    // distance range of a source (closest -> farthest)
    distRange = NormalisableRange<float>(binauraliserNF_getNearfieldLimit_m(hBin), hVst->upperDistRange, 0, 0.5f);
    // pixel radius corresponding to a 45 degree spread: height/4
    iconGrowFac = NormalisableRange<float>(1.0f, (height / 4.0f) / (icon_radius_src * 3.0f));

    setSize (width, height);

    updateSrcLsIconBounds();
    
    showInputs = true;
    showOutputs = true;
	sourceIconIsClicked = false;
    soloActive = false;
    //[/Constructor]
}

pannerView::~pannerView()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void pannerView::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        int x = 0, y = 0, width = 492, height = 246;
        juce::Colour fillColour1 = juce::Colour (0xff4e4e4e), fillColour2 = juce::Colour (0xff202020);
        juce::Colour strokeColour = juce::Colour (0xff9e9e9e);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (juce::ColourGradient (fillColour1,
                                             248.0f - 0.0f + x,
                                             0.0f - 0.0f + y,
                                             fillColour2,
                                             248.0f - 0.0f + x,
                                             248.0f - 0.0f + y,
                                             false));
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    //[UserPaint] Add your own custom painting code here..


    /* Draw Grid lines and labels */
    int numGridLinesX = 8;
    int numGridLinesY = numGridLinesX / 2;
    g.setColour(Colours::white);
    g.setOpacity(0.75f);

    g.drawLine(0.0f, halfHeight, width, halfHeight, 1.0f);
    g.drawLine(halfWidth, 0, halfWidth, height, 1.0f);

    for (int i = 0; i <= numGridLinesX; i++) {
        g.setOpacity(0.1f);
        g.drawLine((float)i*width / (float)numGridLinesX, 0, (float)i*width / (float)numGridLinesX, height, 1.0f);
        g.setOpacity(0.75f);
        if (i <= numGridLinesX / 2) {
            g.drawText(String((int)(360 / 2 - i * (int)360 / numGridLinesX)) + "\xc2\xb0",
                       (float)i*width / (float)numGridLinesX, height / 2, 40, 20, Justification::centred, true);
        }
        else {
            g.drawText(String((int)(360 / 2 - i * (int)360 / numGridLinesX)) + "\xc2\xb0",
                       (float)i*width / (float)numGridLinesX - 40, height / 2, 40, 20, Justification::centred, true);
        }
    }

    for (int i = 0; i <= numGridLinesY; i++) {
        g.setOpacity(0.1f);
        g.drawLine(0, (float)i*height / (float)numGridLinesY, width, (float)i*height / (float)numGridLinesY, 1.0f);
        g.setOpacity(0.75f);
        if (i <= numGridLinesY / 2) {
            g.drawText(String((int)(180 / 2 - i * (int)180 / numGridLinesY)) + "\xc2\xb0",
                       halfWidth, (float)i*height / (float)numGridLinesY, 40, 20, Justification::centred, true);
        }
        else {
            g.drawText(String((int)(180 / 2 - i * (int)180 / numGridLinesY)) + "\xc2\xb0",
                       halfWidth, (float)i*height / (float)numGridLinesY - 20, 40, 20, Justification::centred, true);
        }
    }

    if(showOutputs){
        /* Draw loudspeaker/HRIR icons */
        for(int ls=0; ls<NLoudspeakers; ls++){
            /* icon */
            g.setColour(Colour::fromFloatRGBA(0.5f, 1.0f, 0.1f, 1.0f));
            g.setOpacity(0.3f);
            g.fillEllipse(LoudspeakerIcons[ls]);
        }
    }

    if(showInputs){
        /* Draw Source icons */
        for(int src=0; src<NSources; src++){
            float curDist = binauraliserNF_getSourceDist_m(hBin, src);
            float srcDist_norm = 1.0f - distRange.convertTo0to1(curDist);
            float newDim1 = icon_radius_src * iconGrowFac.convertFrom0to1(srcDist_norm);
            float newDim2 = newDim1 * 2.0f;
            float newDim3 = newDim1 * 3.0f;
            g.setColour(Colour::fromFloatRGBA(1.0f, 0.0f, 1.0f, 0.85f));
            g.setOpacity(0.3f);
            g.fillEllipse(SourceIcons[src].withSizeKeepingCentre(newDim1, newDim1));
            g.fillEllipse(SourceIcons[src].withSizeKeepingCentre(newDim2, newDim2));
            if (curDist < ffThresh) {
                g.setColour(Colour::fromFloatRGBA(255.f/255.f, 183.f/255.f, 3.f/255.f, 0.3f));
            }
            g.fillEllipse(SourceIcons[src].withSizeKeepingCentre(newDim3, newDim3));
        }
        /* Draw Source Icon ID (always on top) */
        g.setColour(Colours::white);
        g.setOpacity(0.9f);
        for(int src=0; src<NSources; src++){
            g.drawText(String(src+1), SourceIcons[src].expanded(10.0f, 0.0f), Justification::centred, true);
        }
    }

    /* Draw SOLO ACTIVE */
    if(soloActive){
        g.setColour(Colours::red);
        g.drawSingleLineText("SoloActive", 5, 15);
    }
    //[/UserPaint]
}

void pannerView::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void pannerView::mouseDown (const juce::MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    for(int i=0; i<NSources; i++){
        Rectangle<int> icon_int;
        icon_int.setBounds(SourceIcons[i].getX(),
                           SourceIcons[i].getY(),
                           SourceIcons[i].getWidth(),
                           SourceIcons[i].getHeight());
        if(icon_int.expanded(4, 4).contains(e.getMouseDownPosition())){
            sourceIconIsClicked = true;
            indexOfClickedSource = i;

            // Solo on ALT
            if(e.mods.isAltDown()){
                binauraliser_setSourceSolo(hBin, i);
                soloActive = true;
            }
            break;
        }
    }
    //[/UserCode_mouseDown]
}

void pannerView::mouseDrag (const juce::MouseEvent& e)
{
    //[UserCode_mouseDrag] -- Add your code here...
    if(sourceIconIsClicked){
        Point<float> point;
        point.setXY((float)e.getPosition().getX()-icon_radius_src, (float)e.getPosition().getY()-icon_radius_src);
        binauraliser_setSourceAzi_deg(hBin, indexOfClickedSource,
                                   ((width - (point.getX() + icon_radius_src)) * 360.0f) / width - 180.0f);
        binauraliser_setSourceElev_deg(hBin, indexOfClickedSource,
                                   ((height - (point.getY() + icon_radius_src)) * 180.0f) / height - 90.0f);
    }

    //[/UserCode_mouseDrag]
}

void pannerView::mouseUp (const juce::MouseEvent& e)
{
    //[UserCode_mouseUp] -- Add your code here...
    sourceIconIsClicked = false;

    // UnSolo on ALT if not clicked on Source
    if(!e.mods.isAltDown()){
        binauraliser_setUnSolo(hBin);
        soloActive = false;
    }
    repaint();
    //[/UserCode_mouseUp]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void pannerView::refreshPanView()
{
    updateSrcLsIconBounds();
    repaint();
}

void pannerView::updateSrcLsIconBounds()
{
    for(int src=0; src<MAX_NUM_INPUTS; src++){
        SourceIcons[src].setBounds(
                                   width - width * (binauraliser_getSourceAzi_deg(hBin, src) + 180.0f) / 360.f - icon_radius_src,
                                   height - height * (binauraliser_getSourceElev_deg(hBin, src) + 90.0f) / 180.0f - icon_radius_src,
                                   icon_diam_src,
                                   icon_diam_src );
    }
    NSources = binauraliser_getNumSources(hBin);
    NLoudspeakers = binauraliser_getNDirs(hBin)>MAX_NUM_OUT_DIRS? MAX_NUM_OUT_DIRS : binauraliser_getNDirs(hBin);
    for(int ls=0; ls<NLoudspeakers; ls++){
        LoudspeakerIcons[ls].setBounds(
                                       width - width * (binauraliser_getHRIRAzi_deg(hBin, ls) + 180.0f) / 360.f - icon_radius_hrir,
                                       height - height * (binauraliser_getHRIRElev_deg(hBin, ls) + 90.0f) / 180.0f - icon_radius_hrir,
                                       icon_diam_hrir,
                                       icon_diam_hrir );
    }
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="pannerView" componentName=""
                 parentClasses="public Component" constructorParams="PluginProcessor* ownerFilter, int _width, int _height"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="492" initialHeight="246">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDrag (const MouseEvent&amp; e)"/>
    <METHOD name="mouseUp (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="323e44">
    <RECT pos="0 0 492 246" fill="linear: 248 0, 248 248, 0=ff4e4e4e, 1=ff202020"
          hasStroke="1" stroke="1, mitered, butt" strokeColour="solid: ff9e9e9e"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

