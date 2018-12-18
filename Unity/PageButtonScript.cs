using UnityEngine;
using System.Collections;

// Attached to an object with selected property
// Worked for OTSprite now
public class PageButtonScript : MonoBehaviour {

    private OTSprite _currentButton;

    public Texture[] _defaultTexture;
    public Texture[] _mouseOverTexture;

    public int ButtonState
    {
        get
        {
            return _buttonState;
        }

        set
        {
            _buttonState = value;

            if (_buttonState > _defaultTexture.Length || _buttonState > _mouseOverTexture.Length )
            {
                _buttonState = Mathf.Min(_defaultTexture.Length, _mouseOverTexture.Length);
            }

            //Debug.Log(_buttonState);

            if (_currentButton != null)
                OnMouseEnterButton(null);
        }
    }

    private int _buttonState = 0;

	// Use this for initialization
	void Start () {
        _currentButton = GetComponent<OTSprite>();

        _currentButton.onMouseEnterOT = OnMouseEnterButton;
        _currentButton.onMouseExitOT = OnMouseExitButton;
        //_currentButton.onInput = OnClick;
	}

    void OnMouseEnterButton(OTObject view)
    {
        if (_buttonState < _mouseOverTexture.Length && _mouseOverTexture[_buttonState] != null)
        {
            _currentButton._image = _mouseOverTexture[_buttonState];
        }
    }

    void OnMouseExitButton(OTObject view)
    {
        if (_buttonState < _defaultTexture.Length && _defaultTexture[_buttonState] != null)
        {
            _currentButton._image = _defaultTexture[_buttonState];
        }
    }
}
