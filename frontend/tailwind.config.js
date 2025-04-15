/** @type {import('tailwindcss').Config} */

const defaultTheme = require("tailwindcss/defaultTheme");
const colors = require("tailwindcss/colors");
const plugin = require("tailwindcss/plugin");

module.exports = {
	mode: "jit",
	content: [
		"./index.html",
		"./src/**/*.{vue,js,ts,jsx,tsx}",
		"./src/**/*.{vue,js,ts,jsx,tsx}",
		"!./src/**/*.spec.{js,ts}",
	],
	safelist: [],
	darkMode: "class",
	theme: {
		extend: {
			fontFamily: {
				raleway: ["Raleway", "sans-serif"],
			},
			screens: {
				sm: { max: "640px" },
				md: { max: "768px" },
				lg: { max: "1024px" },
				xl: { max: "1280px" },
			},
			container: {
				center: true,
			},
			spacing: {
				3.25: "0.8125rem", // 13px
				3.75: "0.938rem", // 15px
				6.25: "1.5625rem", // 25px
				7.5: "1.875rem", // 30px
				12.5: "3.125rem", // 50px
				21.25: "5.3125rem", // 85px
				23.75: "5.9375rem", // 95px
				24.25: "6.0625rem", // 97px
				30.875: "7.6875rem", // 123px
				66.75: "16.6875rem", // 267px
				116.25: "29.0625rem", // 465px
				147.25: "36.8125rem", // 589px
				148.5: "37.125rem", // 594px
			},
			lineHeight: {
				3.25: "13px",
				3.5: "14px",
				3.75: "15px",
				4.5: "18px",
				6.5: "26px",
			},
			colors: {
				primary: {
					50: "#e3f2fc",
					100: "#c7e4f9",
					200: "#a4d3f5",
					300: "#74bdea",
					400: "#3ea1de",
					500: "#0075bf",
					600: "#006aad",
					700: "#005a91",
					800: "#004876",
					900: "#00385f",
				},
				secondary: {
					50: "#e0f7fa",
					100: "#b3ebf5",
					200: "#80deea",
					300: "#4dd0e1",
					400: "#26c6da",
					500: "#00c1e0",
					600: "#00a5c4",
					700: "#0088cc",
					800: "#006ba3",
					900: "#005f8d",
				},
				background: "#edf1f7", // Background Color
				white: colors.white,
				grey: "#eceae8",
			},
			fontSize: {
				xs: ".7rem", // 10px
				sm: ".8rem", // 10px
				"3.5xl": "2rem", // 10px
			},
			height: {
				4.5: "1.125rem", // 18px
				6.5: "1.625rem", // 26px
			},
			width: {
				4.5: "1.125rem", // 18px
				6.5: "1.625rem", // 26px
			},
			objectPosition: {},
			transitionDuration: {
				250: "250ms",
			},
		},
	},
	plugins: [
		plugin(function ({ addVariant, e }) {
			addVariant("active", ({ modifySelectors, separator }) => {
				modifySelectors(({ className }) => {
					return `.active .${e(`active${separator}${className}`)}`;
				});
			});
		}),
	],
};
