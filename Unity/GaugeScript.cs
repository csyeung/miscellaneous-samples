using UnityEngine;
using System.Collections;

public class GaugeScript : MonoBehaviour {

    public OTSprite _gaugePin;
    public OTSprite _gaugeFire;

    public float _startRotation = -90;
    public float _endRotation = 90;

    private float maxValue = 1;
    private float minValue = 0;

    public bool needFireAnimation = false;

    private SpriteMoveForward _script;
    
    // Use this for initialization
	void Start () {
        if (_startRotation > _endRotation)
        {
            float temp = _startRotation;
            _startRotation = _endRotation;
            _endRotation = temp;
        }

        _gaugePin._rotation = _startRotation * -1;
        _script = _gaugeFire.GetComponent<SpriteMoveForward>();
	}
	
    // 0 ~ 100
    public void SetValue(float number)
    {
        if (number < minValue)
            number = minValue;

        if (number == minValue)
            if (_script != null)
                _script.Reset();

        if (number > maxValue)
            number = maxValue;

        if (number == maxValue && needFireAnimation)
            if (_script != null)
                _script.TriggerStart();

        _gaugePin._rotation = ConvertNumberToRotation(number);
    }

    private float ConvertNumberToRotation(float number)
    {
        //OT.Print(number.ToString());
        float totalValue = Mathf.Abs(_startRotation) + Mathf.Abs(_endRotation);
        //OT.Print(totalValue.ToString());
        float result = totalValue * number + _startRotation;
        //OT.Print(result.ToString());
        // HACKHACK: Adjustment of flipped camera
        return result * -1;
    }
}
