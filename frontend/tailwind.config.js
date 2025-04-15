/** @type {import('tailwindcss').Config} */

const defaultTheme = require("tailwindcss/defaultTheme");
const colors = require("tailwindcss/colors");
const plugin = require("tailwindcss/plugin");

module.exports = {
	mode: "jit",
	content: ["./index.html", "./src/**/*.{vue,js,ts,jsx,tsx}", "./src/**/*.{vue,js,ts,jsx,tsx}", "!./src/**/*.spec.{js,ts}"],
	safelist: [],
	darkMode: "class",
	theme: {
		screens: {
			sm: "576px",
			md: "768px",
			lg: "992px",
			xl: "1200px",
		},
		extend: {
			transitionTimingFunction: {
				"soft-in-out": "cubic-bezier(0.42, 0, 0.58, 1)",
				"soft-in": "cubic-bezier(0.42, 0, 1, 1)",
				soft: "cubic-bezier(0.25, 0.1, 0.25, 1)",
			},
			flex: {
				0: "0 0 auto",
			},
			boxShadow: {
				"soft-xl": "0 20px 27px 0 rgba(0, 0, 0, 0.05)",
				"soft-2xl": "0 0.3125rem 0.625rem 0 rgba(0, 0, 0, 0.12)",
				"soft-3xl": "0 8px 26px -4px hsla(0, 0%, 8%, 0.15), 0 8px 9px -5px hsla(0, 0%, 8%, 0.06)",
			},
			fontFamily: {
				raleway: ["Raleway", "sans-serif"],
				awesome: ["FontAwesome"],
			},
			screens: {
				// sm: { max: "640px" },
				// md: { max: "768px" },
				// lg: { max: "1024px" },
				// xl: { max: "1280px" },
			},
			container: {
				center: true,
				padding: "1.5rem", // Standardmäßiges Padding für Container (statt 1.5rem)
			},
			spacing: {
				"-34": "-8.5rem",
				0.5: "0.125rem",
				0.54: "0.135rem",
				0.75: "0.1875rem",
				1.2: "0.3rem",
				5.5: "1.375rem",
				7: "1.75rem",
				2.7: "0.675rem",
				68.5: "17.125rem",
			},
			borderRadius: {
				10: "2.5rem",
			},
			lineHeight: {
				tighter: "1.2",
				default: "1.6",
			},
			colors: {
				white: "rgb(255 255 255 / var(--tw-text-opacity))",
				"black-12.5": "rgb(0 0 0 / 0.125)",
				heading: "#344767",
				"cyan-500": "rgb(23 193 232 / var(--tw-text-opacity))",
			},
			fontSize: {
				xxs: ["0.65rem", { lineHeight: "1rem" }],
			},
			height: {
				19.5: "4.875rem",
			},
			maxWidth: {
				62.5: "15.625rem",
			},
			width: {
				4.5: "1.125rem",
				62.5: "15.625rem",
			},
			objectPosition: {},
			transitionDuration: {
				250: "250ms",
				350: "350ms",
			},
			zIndex: {
				1: "1",
				40: "40",
				100: "100",
			},
		},
	},
	plugins: [
		function ({ addBase }) {
			addBase({
				"h1, h2, h3, h4, h5, h6": {
					marginBottom: "0.5rem",
					color: "#344767",
				},
				h6: {
					fontSize: "1rem",
					lineHeight: "1.625",
				},
				"h4, h5, h6": {
					fontWeight: "600",
				},
				p: {
					lineHeight: "1.625",
					fontWeight: "400",
					marginBottom: "1rem",
				},
			});
		},
		function ({ addUtilities }) {
			addUtilities({
				".h-sidenav": {
					height: "calc(100vh - 199px)",
				},
				".text-inherit": {
					"font-size": "inherit",
				},
				label: {
					display: "inline-block",
				},
				".transform-dropdown-show": {
					transform: "perspective(999px) rotateX(0deg) translateZ(0) translate3d(0, 37px, 5px)",
				},
				".transform-dropdown": {
					transform: "perspective(999px) rotateX(-10deg) translateZ(0) translate3d(0, 37px, 0)",
				},
				".checked\\:after\\:translate-x-5\\.25:checked::after": {
					content: "var(--tw-content)",
					"--tw-translate-x": "1.3125rem",
					transform:
						"translate(var(--tw-translate-x), var(--tw-translate-y)) rotate(var(--tw-rotate)) skewX(var(--tw-skew-x)) skewY(var(--tw-skew-y)) scaleX(var(--tw-scale-x)) scaleY(var(--tw-scale-y))",
				},
				".after\\:translate-x-px::after": {
					content: "var(--tw-content)",
					"--tw-translate-x": "1px",
					transform:
						"translate(var(--tw-translate-x), var(--tw-translate-y)) rotate(var(--tw-rotate)) skewX(var(--tw-skew-x)) skewY(var(--tw-skew-y)) scaleX(var(--tw-scale-x)) scaleY(var(--tw-scale-y))",
				},
				".before\\:content-\\['\\f0d8'\\]::before": {
					"--tw-content": "'\\f0d8'",
					content: "var(--tw-content)",
				},
				".before\\:transition-all::before": {
					transitionProperty: "all",
					transitionTimingFunction: "cubic-bezier(0.4, 0, 0.2, 1)",
					transitionDuration: "150ms",
				},
				".before\\:absolute::before": {
					position: "absolute",
				},
				".before\\:top-0::before": {
					top: "0px",
				},
				".before\\:left-auto::before": {
					left: "auto",
				},
				".before\\:right-7::before": {
					right: "1.75rem",
				},
			});
		},
	],
};
