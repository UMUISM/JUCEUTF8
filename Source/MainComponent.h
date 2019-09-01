#pragma once

class UTF8Component  : public Component
{
public:
    UTF8Component()
    : desc (String(),CharPointer_UTF8("\xe5\x9c\xa8\xe6\xa1\x86\xe4\xb8\xad\xe9\x94\xae\xe5\x85\xa5\xe4\xbb\xbb\xe6\x84\x8f\xe5\xad\x97\xe7\xac\xa6\xe4\xb8\xb2\xef\xbc\x8c\xe5\xae\x83\xe5\xb0\x86\xe5\x9c\xa8\xe4\xb8\x8b\xe9\x9d\xa2\xe6\x98\xbe\xe7\xa4\xba\xe4\xb8\xbaUTF-8\xe6\x96\x87\xe5\xad\x97\xef\xbc\x8c\xe5\x8f\xaf\xe4\xbb\xa5\xe5\x89\xaa\xe5\x88\x87\xe5\xb9\xb6\xe7\xb2\x98\xe8\xb4\xb4\xe5\x88\xb0\xe6\x82\xa8\xe7\x9a\x84\xe6\xba\x90\xe4\xbb\xa3\xe7\xa0\x81\xe4\xb8\xad..."))
    {
        desc.setJustificationType (Justification::centred);
        addAndMakeVisible (desc);
        
        userText.setMultiLine (true, true);
        userText.setReturnKeyStartsNewLine (true);
        addAndMakeVisible (userText);
        userText.onTextChange = [this] { update(); };
        userText.onEscapeKey  = [this] { getTopLevelComponent()->exitModalState (0); };
        
        resultText.setMultiLine (true, true);
        resultText.setReadOnly (true);
        resultText.setSelectAllWhenFocused (true);
        addAndMakeVisible (resultText);
        
        userText.setText (getLastText());
        
        setSize (800, 600);
    }
    
    void update()
    {
        getLastText() = userText.getText();
        resultText.setText (stringLiteral (getLastText(), 100), false);
    }
    
    void resized() override
    {
        auto r = getLocalBounds().reduced (8);
        desc.setBounds (r.removeFromTop (44));
        r.removeFromTop (8);
        userText.setBounds (r.removeFromTop (r.getHeight() / 2));
        r.removeFromTop (8);
        resultText.setBounds (r);
    }
    
    void lookAndFeelChanged() override
    {
        userText.applyFontToAllText (userText.getFont());
        resultText.applyFontToAllText (resultText.getFont());
    }
    
private:
    Label desc;
    TextEditor userText, resultText;
    
    String& getLastText()
    {
        static String t;
        return t;
    }
    
    String stringLiteral (const String& text, int maxLineLength)
    {
        if (text.isEmpty())
            return CharPointer_UTF8 ("\xe6\x97\xa0\xe5\x86\x85\xe5\xae\xb9");
        
        StringArray lines;
        
        {
            auto t = text.getCharPointer();
            bool finished = t.isEmpty();
            
            while (! finished)
            {
                for (auto startOfLine = t;;)
                {
                    switch (t.getAndAdvance())
                    {
                        case 0:     finished = true; break;
                        case '\n':  break;
                        case '\r':  if (*t == '\n') ++t; break;
                        default:    continue;
                    }
                    
                    lines.add (String (startOfLine, t));
                    break;
                }
            }
        }
        
        if (maxLineLength > 0)
        {
            for (int i = 0; i < lines.size(); ++i)
            {
                auto& line = lines.getReference (i);
                
                if (line.length() > maxLineLength)
                {
                    const String start (line.substring (0, maxLineLength));
                    const String end (line.substring (maxLineLength));
                    line = start;
                    lines.insert (i + 1, end);
                }
            }
        }
        
        for (int i = 0; i < lines.size(); ++i)
            lines.getReference(i) = CppTokeniserFunctions::addEscapeChars (lines.getReference(i));
        
        lines.removeEmptyStrings();
        
        for (int i = 0; i < lines.size(); ++i)
            lines.getReference(i) = "\"" + lines.getReference(i) + "\"";
        
        String result (lines.joinIntoString (newLine));
        
        if (! CharPointer_ASCII::isValidString (text.toUTF8(), std::numeric_limits<int>::max()))
            result = "CharPointer_UTF8 (" + result + ")";
        
        return result;
    }
};
