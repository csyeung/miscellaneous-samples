using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class MainLogic : MonoBehaviour {

    // Settings
    public GameObject cameraGroup;

    public int pageWidth = 900;
    public int pageHeight = 1200;
    
    // Page: Actual Page of the Book
    //private int currStage = 0;

    private int currentPage = 1;
    public int Page
    {
        get
        {
            return currentPage;
        }

        set
        {
            currentPage = value;
        }
    }

    // Stage declares number of flips allowed
    public int maxPage = 33;
    public bool disableBackground = false;
    public int orthographicSize = 500;
	private bool isCameraMoving = false;

    private PageButtonScript pbScript;

    public string mainTheme = "";
    private OTSound themeClip;
    public float themeVolume = 0.5f;

	// Use this for initialization
	void Awake () 
    {
        DisableBackground();
        PageButtonCheck(currentPage);
        pbScript.ButtonState = 0;

        CheckOS();
    }

    void DisableBackground()
    {
        if (disableBackground)
        {
            GameObject[] backgroundSprite = GameObject.FindGameObjectsWithTag("Background");

            foreach (GameObject go in backgroundSprite)
            {
                OTSprite sprite = go.GetComponent<OTSprite>();
                sprite.visible = false;
                sprite.enabled = false;
            }
        }
    }

    void CheckOS()
    {
        if (SystemInfo.operatingSystem.Contains("Windows"))
        {
            if (SystemInfo.operatingSystem.Contains("64bit"))
            {
                OT.Print("Win 64bit");
            }
            else
            {
                OT.Print("Win 32bit");
            }
        }
        else if (SystemInfo.operatingSystem.Contains("Mac"))
        {
            OT.Print("Mac");
        }
        else if (SystemInfo.operatingSystem.Contains("Android"))
        {
            OT.Print("Android");
        }
        else if (SystemInfo.operatingSystem.Contains("iPhone"))
        {
            OT.Print("iOS");
        }
    }

    void Start()
    {
        // Update with our default view then Orthello one
        if (Application.platform == RuntimePlatform.Android)
        {
            OT.view.camera.orthographicSize = 700;
        }
        else
        {
            OT.view.camera.orthographicSize = orthographicSize;
        }

        // Background music
        if (mainTheme != null)
        {
            themeClip = new OTSound(mainTheme);
            
            themeClip.Volume(themeVolume);
            themeClip.Loop();
        }
    }

    #region PageChange
    // Testing for flip sensitivity
    public float adjustFactorForFlip = 2f;

    void FixedUpdate()
    {
        Debug.Log(System.GC.GetTotalMemory(false));

        // Support Flip
        if (OT.mobile)
        {
            if (Input.touchCount > 0 && Input.GetTouch(0).phase == TouchPhase.Moved)
            {
                float touchDelta = Input.GetTouch(0).deltaPosition.x;

                if (Mathf.Abs(touchDelta) > adjustFactorForFlip && !isCameraMoving)
                {
                    int pageChange = (touchDelta > 0) ? -2 : 2;
                    PageFlip(pageChange, false);
                }
            }
        }
        else
        {
            if (Input.GetKeyDown(KeyCode.LeftArrow) && !isCameraMoving)
            {
                //FlipPage(false);
                PageFlip(-2, false);
            }

            if (Input.GetKeyDown(KeyCode.RightArrow) && !isCameraMoving)
            {
                //FlipPage(true);
                PageFlip(2, false);
            }
        }
    }

    void Update()
    {
#if UNITY_ANDROID
        if (Application.platform == RuntimePlatform.Android && Input.GetKey(KeyCode.Escape))
        {
            Application.Quit();
        }
#endif
    }

    void OnPageChangeButton(OTObject view)
    {
        bool flag = Constants.GetInputDown();

        if (flag && !isCameraMoving)
        {
            int pageChange = (cameraGroup.transform.position.y > 0) ? -1 : 1;
            PageFlip(pageChange, true);
        }
    }

    // For mobile device debug use
    void OnGUI()
    {
#if UNITY_ANDROID || UNITY_IPHONE
        if (Input.touchCount > 0 && Input.GetTouch(0).phase == TouchPhase.Moved)
        {
            //float touchDelta = Input.GetTouch(0).deltaPosition.x;
            //GUI.Label(new Rect(0, 0, 300, 25), "Touch delta: "+ touchDelta.ToString());
        }
#endif
    }

    void PageFlip(int pageNumber, bool isVertical = true)
    {
        if (Constants.stageLocked) 
            return;

        // Block Page Flip on upper position
        if (currentPage % 2 == 0 && !isVertical)
            return;

        isCameraMoving = true;

        OT.Print(pageNumber.ToString());

        // Handle page change
        int previousPage = currentPage;
        currentPage += pageNumber;

        // Limit max page
        if (currentPage < 1)
        {
            currentPage = 1;
            return;
        }
        else if (currentPage > maxPage)
        {
            currentPage = maxPage;
            return;
        }

        // Page sound trigger
        new OTSound("flip");
        
        // Find target position
        if (isVertical)
        {
            float destPos_y = 0;

            //Debug.Log(pageNumber.ToString());

            if (pageNumber > 0)
            {
                destPos_y = pageHeight;
                pbScript.ButtonState = 1;
            }
            else
            {
                pbScript.ButtonState = 0;
            }

            OT.view.camera.orthographicSize = 0;

            Vector3 updatePosition = cameraGroup.transform.position;
            //updatePosition.x = backObject.transform.position.x;
            updatePosition.y = destPos_y;

            cameraGroup.transform.position = updatePosition;

            OTTween tween = new OTTween(OT.view.camera, 1);
            tween.Tween("orthographicSize", orthographicSize);
            tween.onTweenFinish = OnCameraVertMoveDone;
        }
        else
        {
            Hashtable arguments = new Hashtable();

            float destPos_x = cameraGroup.transform.position.x + ((pageNumber > 0) ? pageWidth : (-pageWidth));
            arguments.Add("x", destPos_x);
            arguments.Add("time", 1.0f);
            arguments.Add("oncompletetarget", gameObject);
            arguments.Add("oncomplete", "OnCameraMovementDone");

            iTween.MoveTo(cameraGroup, arguments);
        }

        PageChange(previousPage, currentPage);
    }

    void OnCameraVertMoveDone(OTTween tween)
    {
        OnCameraMovementDone();
    }
    
    void OnCameraMovementDone()
    {
        isCameraMoving = false;
    }

    #endregion

    #region PageController

    private Dictionary<int, GameObject> objectList;
    private OTSound _page9Dialogue;

    private OTSound page12Clip;
    private OTSound page5Clip;
    private OTSound page7Clip;
    private OTSound page16Clip;

    // Trigger necessary and reset when unused
    void PageChange(int lastPage, int currentPage)
    {
        OT.Print("Current Page: " + currentPage);

        // Starting Part
        if (currentPage == 12)
        {
            SpriteMoveForward script = GameObject.Find("Page 12 Sprite").GetComponent<SpriteMoveForward>();
            script.TriggerStart();

            page12Clip = new OTSound("engine");
            page12Clip.Play();
        }
        else if (currentPage == 10)
        {
            Page10Logic logic = GameObject.Find("Page 10 Logic").GetComponent<Page10Logic>();
            logic.TriggerStart();
        }
        else if (currentPage == 18)
        {
            Page18Logic logic = GameObject.Find("Page 18 Logic").GetComponent<Page18Logic>();
            logic.TriggerStart();
        }
        else if (currentPage == 14)
        {
            Page14Logic logic = GameObject.Find("Page 14 Logic").GetComponent<Page14Logic>();
            logic.TriggerStart();
        }
        else if (currentPage == 20)
        {
            Page20Logic logic = GameObject.Find("Page 20 Logic").GetComponent<Page20Logic>();
            logic.TriggerStart();
        }
        else if (currentPage == 22)
        {
            Page22Logic logic = GameObject.Find("Page 22 Logic").GetComponent<Page22Logic>();
            logic.TriggerStart();
        }
        else if (currentPage == 24)
        {
            Page24Logic logic = GameObject.Find("Page 24 Logic").GetComponent<Page24Logic>();
            logic.TriggerStart();
        }
        else if (currentPage == 28)
        {
            Page28Logic logic = GameObject.Find("Page 28 Logic").GetComponent<Page28Logic>();
            logic.TriggerStart();
        }
        else if (currentPage == 6)
        {
            Page6Logic logic = GameObject.Find("Page 6 Logic").GetComponent<Page6Logic>();
            logic.TriggerStart();
        }
        else if (currentPage == 4)
        {
            Page5Logic logic = GameObject.Find("Page 5 Logic").GetComponent<Page5Logic>();
            logic.TriggerStart();
        }
        else if (currentPage == 16)
        {
            SpriteHighlightSequence logic = GameObject.Find("Page 16 Logic").GetComponent<SpriteHighlightSequence>();
            logic.TriggerStart();

            page16Clip = new OTSound("train");
            page16Clip.Play();
        }
        else if (currentPage == 30)
        {
            Page30Logic logic = GameObject.Find("Page 30 Logic").GetComponent<Page30Logic>();
            logic.TriggerStart();
        }
        else if (currentPage == 25)
        {
            //PuzzleCreate logic = GameObject.Find("Page 27 Logic").GetComponent<PuzzleCreate>();
            //logic.TriggerStart();
        }
        else if (currentPage == 8)
        {
            MatchingLogic logic = GameObject.Find("Matching Logic").GetComponent<MatchingLogic>();
            logic.TriggerStart();
        }
        else if (currentPage == 31)
        {
            Page33Logic logic = GameObject.Find("Page 33 Logic").GetComponent<Page33Logic>();
            logic.TriggerStart();
        }
        else if (currentPage == 19)
        {
            Page21Logic logic = GameObject.Find("Page 21 Logic").GetComponent<Page21Logic>();
            logic.TriggerStart();
        }
        else if (currentPage == 9)
        {
            if (_page9Dialogue != null)
                if (_page9Dialogue.isPlaying)
                    _page9Dialogue.Stop();

            _page9Dialogue = new OTSound("queen");
        }
        else if (currentPage == 5)
        {
            page5Clip = new OTSound("garden");
            page5Clip.Loop();
        }
        else if (currentPage == 7)
        {
            page7Clip = new OTSound("raven-many");
            page7Clip.Loop();
        }

        // Reset Part
        if (lastPage == 12)
        {
            SpriteMoveForward script = GameObject.Find("Page 12 Sprite").GetComponent<SpriteMoveForward>();
            script.Reset();

            if (page12Clip != null)
            {
                page12Clip.Stop();
                page12Clip = null;
            }
        }
        else if (lastPage == 10)
        {
            Page10Logic logic = GameObject.Find("Page 10 Logic").GetComponent<Page10Logic>();
            logic.Reset();
        }
        else if (lastPage == 16)
        {
            SpriteHighlightSequence logic = GameObject.Find("Page 16 Logic").GetComponent<SpriteHighlightSequence>();
            logic.Reset();

            if (page16Clip != null)
            {
                page16Clip.Stop();
                page16Clip = null;
            }
        }
        else if (lastPage == 18)
        {
            Page18Logic logic = GameObject.Find("Page 18 Logic").GetComponent<Page18Logic>();
            logic.Reset();
        }
        else if (lastPage == 14)
        {
            Page14Logic logic = GameObject.Find("Page 14 Logic").GetComponent<Page14Logic>();
            logic.Reset();
        }
        else if (lastPage == 20)
        {
            Page20Logic logic = GameObject.Find("Page 20 Logic").GetComponent<Page20Logic>();
            logic.Reset();
        }
        else if (lastPage == 22)
        {
            Page22Logic logic = GameObject.Find("Page 22 Logic").GetComponent<Page22Logic>();
            logic.Reset();
        }
        else if (lastPage == 24)
        {
            Page24Logic logic = GameObject.Find("Page 24 Logic").GetComponent<Page24Logic>();
            logic.Reset();
        }
        else if (lastPage == 28)
        {
            Page28Logic logic = GameObject.Find("Page 28 Logic").GetComponent<Page28Logic>();
            logic.Reset();
        }
        else if (lastPage == 29)
        {
            Page31Logic logic = GameObject.Find("Page 31 Logic").GetComponent<Page31Logic>();
            logic.Reset();
        }
        else if (lastPage == 6)
        {
            Page6Logic logic = GameObject.Find("Page 6 Logic").GetComponent<Page6Logic>();
            logic.Reset();
        }
        else if (lastPage == 30)
        {
            Page30Logic logic = GameObject.Find("Page 30 Logic").GetComponent<Page30Logic>();
            logic.Reset();
        }
        else if (lastPage == 25)
        {
            PuzzleCreate logic = GameObject.Find("Page 27 Logic").GetComponent<PuzzleCreate>();
            logic.Reset();
        }
        else if (lastPage == 8)
        {
            MatchingLogic logic = GameObject.Find("Matching Logic").GetComponent<MatchingLogic>();
            logic.Reset();
        }
        else if (lastPage == 4)
        {
            Page5Logic logic = GameObject.Find("Page 5 Logic").GetComponent<Page5Logic>();
            logic.Reset();
        }
        else if (lastPage == 31)
        {
            Page33Logic logic = GameObject.Find("Page 33 Logic").GetComponent<Page33Logic>();
            logic.Reset();
        }
        else if (lastPage == 5)
        {
            if (page5Clip != null)
            {
                page5Clip.Stop();
                page5Clip = null;
            }
        }
        else if (lastPage == 7)
        {
            if (page7Clip != null)
            {
                page7Clip.Stop();
                page7Clip = null;
            }
        }

        PageButtonCheck(currentPage);
    }

    #endregion

    #region PageLockControl
    public OTSprite _pageButton;
    public int[] lockedPageList;

    private void PageButtonCheck(int stage)
    {
        bool isActivated = !PageLocked(stage);

        _pageButton.visible = isActivated;
        _pageButton.registerInput = isActivated;
        _pageButton.onInput = OnPageChangeButton;

        pbScript = _pageButton.GetComponent<PageButtonScript>();
    }

    private bool PageLocked(int stage)
    {
        foreach (int locked in lockedPageList)
        {
            if (locked == stage)
                return true;
        }

        return false;
    }
    #endregion
}
